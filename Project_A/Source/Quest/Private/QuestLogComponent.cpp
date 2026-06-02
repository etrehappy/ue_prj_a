
#include "QuestLogComponent.h"

#include "GameFramework/Pawn.h"
#include "Inventory.h"
#include "InventoryComponent.h"
#include "Net/UnrealNetwork.h"
#include "QuestDefinition.h"
#include "QuestManagerSubsystem.h"
#include "TimerManager.h"

#include "ProjectALog.h"


UQuestLogComponent::UQuestLogComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UQuestLogComponent::BeginPlay()
{
	Super::BeginPlay();

	BindInventoryEvents();
	SyncInventoryBackedObjectives();
	StartInventoryBindRetry();

	ValidateTrackedQuest();
	BroadcastQuestState();
}
void UQuestLogComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UQuestLogComponent, QuestInstances);
}

const FQuestInstance* UQuestLogComponent::GetQuestInstance(FName QuestId) const
{
	return FindQuestInstance(QuestId);
}

void UQuestLogComponent::OnRep_QuestInstances()
{
	ValidateTrackedQuest();
	BroadcastQuestState();
}

void UQuestLogComponent::BroadcastQuestState() const
{
	OnQuestLogUpdated.Broadcast();
}

void UQuestLogComponent::ValidateTrackedQuest()
{
	if (TrackedQuestId.IsNone())
	{
		return;
	}

	const bool bKeepTracked = IsQuestAccepted(TrackedQuestId) || IsQuestReadyToTurnIn(TrackedQuestId);
	if (!bKeepTracked)
	{
		TrackedQuestId = NAME_None;
	}
}

void UQuestLogComponent::AcceptQuestFromDefinition(const FQuestInstance& InitialInstance)
{
	if (!IsRunningDedicatedServer())
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - should only be called on server", FString(__FUNCTION__));
		return;
	}

	if (InitialInstance.QuestId.IsNone())
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - InitialInstance.QuestId is none", FString(__FUNCTION__));
		return;
	}

	FQuestInstance* Existing = FindQuestInstanceMutable(InitialInstance.QuestId);
	if (Existing)
	{
		// If already accepted/completed/turned-in then ignore
		if (Existing->Status == EQuestStatus::Accepted || Existing->Status == EQuestStatus::Completed || Existing->Status == EQuestStatus::TurnedIn)
		{
			UE_LOGFMT(LogProjectA, Verbose, "{0} - Quest already accepted/completed/turned in: {1}", FString(__FUNCTION__), *InitialInstance.QuestId.ToString());
			return;
		}

		*Existing = InitialInstance;
		if (Existing->Status == EQuestStatus::None)
		{
			Existing->Status = EQuestStatus::Accepted;
		}
	}
	else
	{
		QuestInstances.Add(InitialInstance);
	}

	SyncInventoryBackedObjectives();
	BroadcastQuestState();
}

bool UQuestLogComponent::UpdateObjectiveProgress(FName QuestId, FGameplayTag ObjectiveTag, int32 Delta)
{
	// 1. Validation
	//UE_LOGFMT(LogProjectA, Log, "{0} - QuestId={1}, ObjectiveTag={2}, Delta={3}", FString(__FUNCTION__), *QuestId.ToString(), *ObjectiveTag.ToString(), Delta);

	if (!IsRunningDedicatedServer())
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - should only be called on server", FString(__FUNCTION__));
		return false;
	}

	if (QuestId.IsNone() || !ObjectiveTag.IsValid())
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Invalid parameters", FString(__FUNCTION__));
		return false;
	}

	FQuestInstance* Instance = FindQuestInstanceMutable(QuestId);
	if (!Instance)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Quest {1} not found", FString(__FUNCTION__), *QuestId.ToString());
		return false;
	}

	// Do not progress objectives for completed or already turned-in quests.
	if (Instance->Status == EQuestStatus::Completed || Instance->Status == EQuestStatus::TurnedIn)
	{
		UE_LOGFMT(LogProjectA, Verbose, "{0} - Quest {1} already completed/turned in", FString(__FUNCTION__), *QuestId.ToString());
		return false;
	}


	//2. Find the objective and update progress

	for (FQuestObjectiveRuntime& Obj : Instance->Objectives)
	{	
		if (!Obj.ObjectiveTag.MatchesTagExact(ObjectiveTag))
		{
			continue;
		}

		const int32 Prev = Obj.CurrentValue;
		Obj.CurrentValue = FMath::Clamp(Obj.CurrentValue + Delta, 0, Obj.TargetValue);
				
		if (Obj.CurrentValue == Prev)
		{
			// nothing changed
			return false;
		}
				
		bool bAllComplete = true;
		for (const FQuestObjectiveRuntime& O : Instance->Objectives)
		{
			if (O.CurrentValue < O.TargetValue)
			{
				// At least one objective is not complete, so the quest is not complete.
				bAllComplete = false;
				break;
			}
		}

		if (bAllComplete)
		{
			Instance->Status = EQuestStatus::Completed;
			ValidateTrackedQuest();
		}

		BroadcastQuestState();
		return true;
	}

	UE_LOGFMT(LogProjectA, Warning, "{0} - Objective with tag {1} not found for quest {2}", FString(__FUNCTION__), *ObjectiveTag.ToString(), *QuestId.ToString());
	return false;
}

bool UQuestLogComponent::IsQuestAccepted(FName QuestId) const
{
	const FQuestInstance* Instance = FindQuestInstance(QuestId);
	return Instance && Instance->Status == EQuestStatus::Accepted;
}

bool UQuestLogComponent::IsQuestCompleted(FName QuestId) const
{
	const FQuestInstance* Instance = FindQuestInstance(QuestId);	
	return Instance && (Instance->Status == EQuestStatus::Completed || Instance->Status == EQuestStatus::TurnedIn);
}

void UQuestLogComponent::SetTrackedQuestId(FName InQuestId)
{
	if (InQuestId.IsNone())
	{
		TrackedQuestId = NAME_None;
		BroadcastQuestState();
		return;
	}

	if (!IsQuestAccepted(InQuestId))
	{
		UE_LOGFMT(LogProjectA, Verbose, "{0} - Quest is not accepted: {1}", FString(__FUNCTION__), *InQuestId.ToString());
		return;
	}

	TrackedQuestId = InQuestId;
	BroadcastQuestState();
}

FName UQuestLogComponent::GetTrackedOrFirstAcceptedQuestId() const
{
	if (!TrackedQuestId.IsNone() && (IsQuestAccepted(TrackedQuestId) || IsQuestReadyToTurnIn(TrackedQuestId)))
	{
		return TrackedQuestId;
	}

	for (const FQuestInstance& Instance : QuestInstances)
	{
		if (Instance.Status == EQuestStatus::Accepted || Instance.Status == EQuestStatus::Completed)
		{
			return Instance.QuestId;
		}
	}

	return NAME_None;
}

const FQuestInstance* UQuestLogComponent::FindQuestInstance(FName QuestId) const
{
	if (QuestId.IsNone())
	{
		return nullptr;
	}

	for (const FQuestInstance& Instance : QuestInstances)
	{
		if (Instance.QuestId == QuestId)
		{
			return &Instance;
		}
	}
	return nullptr;
}

FQuestInstance* UQuestLogComponent::FindQuestInstanceMutable(FName QuestId)
{
	if (QuestId.IsNone())
	{
		return nullptr;
	}

	for (FQuestInstance& Instance : QuestInstances)
	{
		if (Instance.QuestId == QuestId)
		{
			return &Instance;
		}
	}
	return nullptr;
}

bool UQuestLogComponent::TryTurnInQuest(FName QuestId)
{
	if (!IsRunningDedicatedServer())
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - should only be called on server", FString(__FUNCTION__));
		return false;
	}

	FQuestInstance* Instance = FindQuestInstanceMutable(QuestId);
	if (!Instance)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Quest {1} not found", FString(__FUNCTION__), *QuestId.ToString());
		return false;
	}

	if (Instance->Status != EQuestStatus::Completed)
	{
		UE_LOGFMT(LogProjectA, Verbose, "{0} - Quest {1} is not ready to turn in", FString(__FUNCTION__), *QuestId.ToString());
		return false;
	}

	Instance->Status = EQuestStatus::TurnedIn;
	ValidateTrackedQuest();
	BroadcastQuestState();
	return true;
}

bool UQuestLogComponent::IsQuestReadyToTurnIn(FName QuestId) const
{
	const FQuestInstance* Instance = FindQuestInstance(QuestId);
	return Instance && Instance->Status == EQuestStatus::Completed;
}

bool UQuestLogComponent::IsQuestTurnedIn(FName QuestId) const
{
	const FQuestInstance* Instance = FindQuestInstance(QuestId);
	return Instance && Instance->Status == EQuestStatus::TurnedIn;
}

void UQuestLogComponent::BindInventoryEvents()
{
	// 1. Validation

	if(!IsRunningDedicatedServer())
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - should only be called on server", FString(__FUNCTION__));
		return;
	}

	AActor* OwnerActor = GetOwner();
	if (!OwnerActor)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Owner actor not found", FString(__FUNCTION__));
		return;
	}

	APawn* OwnerPawn = Cast<APawn>(OwnerActor);
	if (!OwnerPawn)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Owner is not a pawn", FString(__FUNCTION__));
		return;
	}

	UInventoryComponent* InventoryComponent = OwnerPawn->FindComponentByClass<UInventoryComponent>();
	if (!InventoryComponent)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Inventory component not found", FString(__FUNCTION__));
		return;
	}

	UInventory* Inventory = InventoryComponent->GetInventory();
	if (!Inventory)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Inventory not found", FString(__FUNCTION__));
		return;
	}

	// 2. Bind events
	
	OwnerInventoryComponent = InventoryComponent;

	Inventory->OnInventoryChanged.RemoveAll(this);
	Inventory->OnInventoryChanged.AddUniqueDynamic(this, &UQuestLogComponent::HandleOwnedInventoryChanged);
}

void UQuestLogComponent::HandleOwnedInventoryChanged()
{
	if (!IsRunningDedicatedServer())
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - should only be called on server", FString(__FUNCTION__));
		return;
	}

	SyncInventoryBackedObjectives();
}

void UQuestLogComponent::SyncInventoryBackedObjectives()
{
	// 1. Validation
	if (!IsRunningDedicatedServer())
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - should only be called on server", FString(__FUNCTION__));
		return;
	}

	if (!OwnerInventoryComponent || !OwnerInventoryComponent->GetInventory())
	{
		BindInventoryEvents();
	}

	if (!OwnerInventoryComponent || !OwnerInventoryComponent->GetInventory())
	{
		return;
	}

	UWorld* World = GetWorld();
	UQuestManagerSubsystem* QuestManagerSubsystem = World ? World->GetSubsystem<UQuestManagerSubsystem>() : nullptr;
	if (!QuestManagerSubsystem)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - QuestManagerSubsystem not found", FString(__FUNCTION__));
		return;
	}

	// 2. Iterate accepted quests
	for (const FQuestInstance& QuestInstance : QuestInstances)
	{
		if (QuestInstance.Status != EQuestStatus::Accepted)
		{
			continue;
		}

		const UQuestDefinition* QuestDefinition = QuestManagerSubsystem->GetQuestDefinition(QuestInstance.QuestId);
		if (!QuestDefinition)
		{
			continue;
		}

		// 3. Iterate objectives for this quest
		for (const FQuestObjectiveRuntime& RuntimeObjective : QuestInstance.Objectives)
		{
			// 4. Find inventory-backed ones to sync
			const FQuestObjectiveDefinition* ObjectiveDefinition = FindObjectiveDefinitionByTag(QuestDefinition, RuntimeObjective.ObjectiveTag);
			if (!ObjectiveDefinition)
			{
				continue;
			}

			if (ObjectiveDefinition->ObjectiveKind != EQuestObjectiveKind::InventoryItemCount)
			{
				continue;
			}

			if (!ObjectiveDefinition->RequiredItemTypeTag.IsValid())
			{
				continue;
			}

			// Do not regress already completed objective.
			if (RuntimeObjective.CurrentValue >= RuntimeObjective.TargetValue)
			{
				continue;
			}

			// 5. Get current count and apply only positive delta.
			const int32 DesiredCount = OwnerInventoryComponent->GetItemCountByTag(ObjectiveDefinition->RequiredItemTypeTag);
			const int32 PositiveDelta = FMath::Max(0, DesiredCount - RuntimeObjective.CurrentValue);
			if (PositiveDelta == 0)
			{
				continue;
			}

			// 6. Update objective progress.
			UpdateObjectiveProgress(QuestInstance.QuestId, RuntimeObjective.ObjectiveTag, PositiveDelta);
		}
	}
}
void UQuestLogComponent::StartInventoryBindRetry()
{
	if (!IsRunningDedicatedServer())
	{
		UE_LOGFMT(LogProjectA, Log, "{0} - should only be called on server", FString(__FUNCTION__));
		return;
	}
	UWorld* World = GetWorld();
	if (!World) { return; }

	World->GetTimerManager().SetTimer(InventoryBindRetryTimer, this,
		&UQuestLogComponent::RetryBindInventoryEvents,
		0.25f, true);
}

const FQuestObjectiveDefinition* UQuestLogComponent::FindObjectiveDefinitionByTag(const UQuestDefinition* QuestDefinition, const FGameplayTag& ObjectiveTag)
{
	if (!QuestDefinition)
	{
		return nullptr;
	}

	for (const FQuestObjectiveDefinition& Def : QuestDefinition->Objectives)
	{
		if (Def.ObjectiveTag.MatchesTagExact(ObjectiveTag))
		{
			return &Def;
		}
	}

	return nullptr;
}

void UQuestLogComponent::RetryBindInventoryEvents()
{
	if (!IsRunningDedicatedServer())
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - should only be called on server", FString(__FUNCTION__));
		return;
	}

	BindInventoryEvents();

	if (OwnerInventoryComponent && OwnerInventoryComponent->GetInventory())
	{
		UWorld* World = GetWorld();
		if (World)
		{
			World->GetTimerManager().ClearTimer(InventoryBindRetryTimer);
		}

		SyncInventoryBackedObjectives();
	}
}

void UQuestLogComponent::RestoreFromSnapshot(const TArray<FQuestInstance>& Snapshot)
{
	if (!IsRunningDedicatedServer())
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - should only be called on server", FString(__FUNCTION__));
		return;
	}

	QuestInstances = Snapshot;
	ValidateTrackedQuest();
	SyncInventoryBackedObjectives();
	BroadcastQuestState();
}
#include "QuestManagerSubsystem.h"

#include "Engine/World.h"
#include "GameFramework/Controller.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "NetPlayerState.h"
#include "QuestDefinition.h"
#include "QuestLogComponent.h"

#include "ProjectALog.h"

void UQuestManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	QuestDefinitionsById.Reset();
	LoadQuestDefinitionsFromConfig();
}

void UQuestManagerSubsystem::Deinitialize()
{
	QuestDefinitionsById.Reset();
	Super::Deinitialize();
}

void UQuestManagerSubsystem::LoadQuestDefinitionsFromConfig()
{
	for (const TSoftObjectPtr<UQuestDefinition>& QuestDefinitionSoft : StartupQuestDefinitions)
	{
		if (QuestDefinitionSoft.IsNull())
		{
			continue;
		}

		UQuestDefinition* QuestDefinition = QuestDefinitionSoft.LoadSynchronous();
		if (!QuestDefinition)
		{
			UE_LOGFMT(LogProjectA, Warning, "{0} - Failed to load quest definition from config", FString(__FUNCTION__));
			continue;
		}

		RegisterQuestDefinition(QuestDefinition);
	}
}

void UQuestManagerSubsystem::RegisterQuestDefinition(UQuestDefinition* QuestDefinition)
{
	if (!QuestDefinition || QuestDefinition->QuestId.IsNone())
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Invalid QuestDefinition or QuestId", FString(__FUNCTION__));
		return;
	}

	QuestDefinitionsById.Add(QuestDefinition->QuestId, QuestDefinition);
}

const UQuestDefinition* UQuestManagerSubsystem::GetQuestDefinition(FName QuestId) const
{
	if (QuestId.IsNone())
	{
		return nullptr;
	}

	const TObjectPtr<UQuestDefinition>* QuestDefinitionPtr = QuestDefinitionsById.Find(QuestId);
	return QuestDefinitionPtr ? QuestDefinitionPtr->Get() : nullptr;
}

bool UQuestManagerSubsystem::TryAcceptQuest(APawn* RequesterPawn, FName QuestId)
{
	// 1. Validation
	if (!RequesterPawn || !IsRunningDedicatedServer() || QuestId.IsNone())
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Invalid params or no authority", FString(__FUNCTION__));
		return false;
	}

	const UQuestDefinition* QuestDefinition = GetQuestDefinition(QuestId);
	if (!QuestDefinition)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - QuestDefinition not found for QuestId={1}", FString(__FUNCTION__), QuestId.ToString());
		return false;
	}

	FQuestInstance InitialInstance{};
	if (!QuestDefinition->BuildInitialQuestInstance(InitialInstance))
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Failed to build quest instance for QuestId={1}", FString(__FUNCTION__), QuestId.ToString());
		return false;
	}

	TArray<APawn*> Recipients{};
	if (!FindRecipients(RequesterPawn, QuestDefinition->SharingMode, Recipients))
	{
		return false;
	}

	FilterRecipientsForQuestAccept(QuestId, Recipients);

	if (Recipients.Num() == 0)
	{
		UE_LOGFMT(LogProjectA, Verbose, "{0} - No eligible recipients for QuestId={1}", FString(__FUNCTION__), QuestId.ToString());
		return false;
	}

	// 2. Apply quest acceptance to recipients

	bool bWasApplied = false;
	for (APawn* RecipientPawn : Recipients)
	{
		UQuestLogComponent* QuestLogComponent = GetQuestLogComponent(RecipientPawn);
		if (!QuestLogComponent)
		{
			continue;
		}

		QuestLogComponent->AcceptQuestFromDefinition(InitialInstance);
		bWasApplied = true;
	}

	return bWasApplied;
}

bool UQuestManagerSubsystem::TryAdvanceObjective(APawn* InstigatorPawn, FName QuestId, FGameplayTag ObjectiveTag, int32 Delta)
{
	//1. Validation
	if (!InstigatorPawn || !IsRunningDedicatedServer() || QuestId.IsNone() || !ObjectiveTag.IsValid() || Delta == 0)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Invalid params or no authority", FString(__FUNCTION__));
		return false;
	}

	UQuestLogComponent* InstigatorQuestLog = GetQuestLogComponent(InstigatorPawn);
	if (!InstigatorQuestLog)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Instigator pawn does not have a quest log component", FString(__FUNCTION__));
		return false;
	}

	const FQuestInstance* InstigatorInstance = InstigatorQuestLog->GetQuestInstance(QuestId);
	if (!InstigatorInstance)
	{
		UE_LOGFMT(LogProjectA, Verbose, "{0} - Quest instance not found for QuestId={1}", FString(__FUNCTION__), QuestId.ToString());
		return false;
	}

	TArray<APawn*> Recipients{};
	if (!FindRecipients(InstigatorPawn, InstigatorInstance->SharingMode, Recipients))
	{
		return false;
	}

	FilterRecipientsForQuestProgress(QuestId, Recipients);

	if (Recipients.Num() == 0)
	{
		UE_LOGFMT(LogProjectA, Verbose, "{0} - No eligible recipients for progress update, QuestId={1}", FString(__FUNCTION__), QuestId.ToString());
		return false;
	}

	const UQuestDefinition* QuestDefinition = GetQuestDefinition(QuestId);
	const bool bAutoTurnInWithoutAcceptor = QuestDefinition && QuestDefinition->QuestAcceptor.IsNone();


	// 2. Apply objective progress updates to recipients

	bool bAnyUpdated = false;
	for (APawn* RecipientPawn : Recipients)
	{
		UQuestLogComponent* QuestLogComponent = GetQuestLogComponent(RecipientPawn);
		if (!QuestLogComponent)
		{
			continue;
		}

		const bool bUpdatedThisPawn = QuestLogComponent->UpdateObjectiveProgress(QuestId, ObjectiveTag, Delta);
		bAnyUpdated = bAnyUpdated || bUpdatedThisPawn;

		if (bAutoTurnInWithoutAcceptor && QuestLogComponent->IsQuestReadyToTurnIn(QuestId))
		{
			QuestLogComponent->TryTurnInQuest(QuestId);
		}
	}

	return bAnyUpdated;
}

bool UQuestManagerSubsystem::TryTurnInQuest(APawn* RequesterPawn, FName QuestId)
{
	// 1. Validation
	if (!RequesterPawn || !IsRunningDedicatedServer() || QuestId.IsNone())
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Invalid params or no authority", FString(__FUNCTION__));
		return false;
	}

	UQuestLogComponent* RequesterQuestLog = GetQuestLogComponent(RequesterPawn);
	if (!RequesterQuestLog)
	{
		return false;
	}

	const FQuestInstance* RequesterInstance = RequesterQuestLog->GetQuestInstance(QuestId);
	if (!RequesterInstance)
	{
		UE_LOGFMT(LogProjectA, Verbose, "{0} - Quest instance not found for QuestId={1}", FString(__FUNCTION__), QuestId.ToString());
		return false;
	}

	TArray<APawn*> Recipients{};
	if (!FindRecipients(RequesterPawn, RequesterInstance->SharingMode, Recipients))
	{
		return false;
	}

	FilterRecipientsForQuestTurnIn(QuestId, Recipients);

	if (Recipients.Num() == 0)
	{
		UE_LOGFMT(LogProjectA, Verbose, "{0} - No eligible recipients for turn in, QuestId={1}", FString(__FUNCTION__), QuestId.ToString());
		return false;
	}

	// 2. Apply quest turn in to recipients

	bool bAnyTurnedIn = false;
	for (APawn* RecipientPawn : Recipients)
	{
		UQuestLogComponent* QuestLogComponent = GetQuestLogComponent(RecipientPawn);
		if (!QuestLogComponent)
		{
			continue;
		}
				
		const bool bUpdatedThisPawn = QuestLogComponent->TryTurnInQuest(QuestId);
		bAnyTurnedIn = bAnyTurnedIn || bUpdatedThisPawn;
	}

	return bAnyTurnedIn;
}

UQuestLogComponent* UQuestManagerSubsystem::GetQuestLogComponent(const APawn* Pawn) const
{
	return Pawn ? Pawn->FindComponentByClass<UQuestLogComponent>() : nullptr;
}

bool UQuestManagerSubsystem::FindRecipients(APawn* SourcePawn, EQuestSharingMode SharingMode, TArray<APawn*>& OutRecipients) const
{
	OutRecipients.Reset();

	// 1. Validation
	if (!SourcePawn)
	{
		return false;
	}

	if (SharingMode == EQuestSharingMode::Individual)
	{
		OutRecipients.Add(SourcePawn);
		return true;
	}

	const ANetPlayerState* SourcePlayerState = SourcePawn->GetPlayerState<ANetPlayerState>();
	if (!SourcePlayerState || SourcePlayerState->GetPartyId().IsNone())
	{
		OutRecipients.Add(SourcePawn);
		return true;
	}

	const UWorld* World = GetWorld();
	const AGameStateBase* GameState = World ? World->GetGameState() : nullptr;
	if (!GameState)
	{
		OutRecipients.Add(SourcePawn);
		return true;
	}

	//2. Find all party members and add their pawns to recipients

	const FName PartyId = SourcePlayerState->GetPartyId();

	for (APlayerState* PlayerStateBase : GameState->PlayerArray)
	{
		const ANetPlayerState* NetPlayerState = Cast<ANetPlayerState>(PlayerStateBase);
		if (!NetPlayerState || NetPlayerState->GetPartyId() != PartyId)
		{
			continue;
		}

		AController* OwnerController = Cast<AController>(NetPlayerState->GetOwner());
		APawn* MemberPawn = OwnerController ? OwnerController->GetPawn() : nullptr;
		if (!MemberPawn)
		{
			continue;
		}

		OutRecipients.AddUnique(MemberPawn);
	}

	if (OutRecipients.Num() == 0)
	{
		OutRecipients.Add(SourcePawn);
	}

	return true;
}


void UQuestManagerSubsystem::FilterRecipientsForQuestAccept(FName QuestId, TArray<APawn*>& OutRecipients) const
{
	TArray<APawn*> Filtered;
	Filtered.Reserve(OutRecipients.Num());

	for (APawn* Pawn : OutRecipients)
	{
		const UQuestLogComponent* QuestLogComponent = GetQuestLogComponent(Pawn);
		if (!QuestLogComponent)
		{
			continue;
		}
				
		if (QuestLogComponent->IsQuestAccepted(QuestId) || QuestLogComponent->IsQuestCompleted(QuestId))
		{
			continue;
		}

		Filtered.Add(Pawn);
	}

	OutRecipients = MoveTemp(Filtered);
}


void UQuestManagerSubsystem::FilterRecipientsForQuestProgress(FName QuestId, TArray<APawn*>& OutRecipients) const
{
	TArray<APawn*> Filtered;
	Filtered.Reserve(OutRecipients.Num());

	for (APawn* Pawn : OutRecipients)
	{
		const UQuestLogComponent* QuestLogComponent = GetQuestLogComponent(Pawn);
		if (!QuestLogComponent)
		{
			continue;
		}

		const FQuestInstance* QuestInstance = QuestLogComponent->GetQuestInstance(QuestId);
		if (!QuestInstance)
		{
			continue;
		}

		if (QuestInstance->Status != EQuestStatus::Accepted)
		{
			continue;
		}

		Filtered.Add(Pawn);
	}

	OutRecipients = MoveTemp(Filtered);
}


void UQuestManagerSubsystem::FilterRecipientsForQuestTurnIn(FName QuestId, TArray<APawn*>& OutRecipients) const
{
	TArray<APawn*> Filtered;
	Filtered.Reserve(OutRecipients.Num());

	for (APawn* Pawn : OutRecipients)
	{
		const UQuestLogComponent* QuestLogComponent = GetQuestLogComponent(Pawn);
		if (!QuestLogComponent)
		{
			continue;
		}

		if (!QuestLogComponent->IsQuestReadyToTurnIn(QuestId))
		{
			continue;
		}

		Filtered.Add(Pawn);
	}

	OutRecipients = MoveTemp(Filtered);
}
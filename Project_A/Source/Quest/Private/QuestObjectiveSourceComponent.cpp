

#include "QuestObjectiveSourceComponent.h"

#include "InventoryComponent.h"
#include "Net/UnrealNetwork.h"
#include "QuestLogComponent.h"
#include "QuestManagerSubsystem.h"

#include "ProjectALog.h"

UQuestObjectiveSourceComponent::UQuestObjectiveSourceComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UQuestObjectiveSourceComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UQuestObjectiveSourceComponent, bIsConsumed);
}

bool UQuestObjectiveSourceComponent::CanExecute(APawn* Interactor) const
{
	if (!Interactor || QuestId.IsNone() || !ObjectiveTag.IsValid())
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Invalid parameters or configuration", FString(__FUNCTION__));
		return false;
	}

	if (bOneShot && bIsConsumed)
	{
		//UE_LOGFMT(LogProjectA, Log, "{0} - Already consumed one-shot objective source", FString(__FUNCTION__));
		return false;
	}

	if (!bRequireQuestAccepted)
	{
		//UE_LOGFMT(LogProjectA, Log, "{0} - No need to check quest acceptance", FString(__FUNCTION__));
		return true;
	}

	const UQuestLogComponent* QuestLogComponent = Interactor->FindComponentByClass<UQuestLogComponent>();
	return QuestLogComponent && QuestLogComponent->IsQuestAccepted(QuestId);
}

void UQuestObjectiveSourceComponent::BuildActions(APawn* Interactor, TArray<FInteractionActionType>& OutActions) const
{
	OutActions.Reset();

	const bool bCanExecuteNow = CanExecute(Interactor);

	for (const TObjectPtr<UInteractionActionDefinition>& ActionDef : InteractionActions)
	{
		if (!ActionDef || !ActionDef->ActionTag.IsValid())
		{
			continue;
		}

		FInteractionActionType ActionView{};
		ActionView.bIsEnabled = bCanExecuteNow && ActionDef->bEnabledByDefault;
		ActionView.Definition = ActionDef;
		OutActions.Add(ActionView);
	}
}

bool UQuestObjectiveSourceComponent::TryExecute(APawn* Interactor, FGameplayTag ActionTag)
{
	// 1. Validation	
	if (!IsRunningDedicatedServer())
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Should run on authority only", FString(__FUNCTION__));
		return false;
	}

	if (!CanExecute(Interactor) || !IsActionAllowed(ActionTag))
	{
		UE_LOGFMT(LogProjectA, Log, "{0} - Cannot execute action with tag {1}", FString(__FUNCTION__), ActionTag.ToString());
		return false;
	}

	if (bConsumeRequiredItem)
	{
		UInventoryComponent* InventoryComponent = Interactor ? Interactor->FindComponentByClass<UInventoryComponent>() : nullptr;
		if (!InventoryComponent || !InventoryComponent->ConsumeItemsByTag(RequiredItemTag, RequiredItemCount))
		{
			UE_LOGFMT(LogProjectA, Log, "{0} - Failed to consume required items with tag {1} and count {2}", FString(__FUNCTION__), RequiredItemTag.ToString(), RequiredItemCount);
			return false;
		}
	}

	UQuestManagerSubsystem* QuestManagerSubsystem = GetWorld() ? GetWorld()->GetSubsystem<UQuestManagerSubsystem>() : nullptr;
	if (!QuestManagerSubsystem)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - QuestManagerSubsystem not found", FString(__FUNCTION__));
		return false;
	}


	// 2. Update quest progress

	const bool bWasUpdated = QuestManagerSubsystem->TryAdvanceObjective(Interactor, QuestId, ObjectiveTag, ObjectiveDelta);
	if (!bWasUpdated)
	{
		return false;
	}

	if (bOneShot)
	{
		bIsConsumed = true;
		OnObjectiveSourceConsumed();
		OnConsumed.Broadcast();
	}

	return true;
}

bool UQuestObjectiveSourceComponent::IsActionAllowed(FGameplayTag ActionTag) const
{
	if (!ActionTag.IsValid())
	{
		return false;
	}

	for (const TObjectPtr<UInteractionActionDefinition>& ActionDef : InteractionActions)
	{
		if (!ActionDef || !ActionDef->ActionTag.IsValid())
		{
			continue;
		}

		if (ActionDef->ActionTag.MatchesTagExact(ActionTag))
		{
			return ActionDef->bEnabledByDefault;
		}
	}

	return false;
}

void UQuestObjectiveSourceComponent::OnRep_IsConsumed()
{
	if (bIsConsumed)
	{	
		OnObjectiveSourceConsumed();
				
		OnConsumed.Broadcast();
	}
}
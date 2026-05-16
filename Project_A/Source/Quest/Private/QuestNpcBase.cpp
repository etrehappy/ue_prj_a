#include "QuestNpcBase.h"
#include "QuestLogComponent.h"
#include "TagList.h"
#include "ProjectALog.h"

bool AQuestNpcBase::CanInteract(APawn* Interactor) const
{
	if (!Interactor)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Interactor = nullptr", FString(__FUNCTION__));
		return false;
	}

	const UQuestLogComponent* QuestLogComponent = Interactor->FindComponentByClass<UQuestLogComponent>();
	if (!QuestLogComponent)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Interactor does not have a QuestLogComponent", FString(__FUNCTION__));
		return false;
	}

	return !QuestLogComponent->IsInsertStoneQuestCompleted();
}

void AQuestNpcBase::Interact(APawn* Interactor)
{
	if (!Interactor || !IsRunningDedicatedServer())
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Interactor is invalid or does not have authority", FString(__FUNCTION__));
		return;
	}

	UQuestLogComponent* QuestLogComponent = Interactor->FindComponentByClass<UQuestLogComponent>();
	if (!QuestLogComponent)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Interactor does not have a QuestLogComponent", FString(__FUNCTION__));
		return;
	}

	QuestLogComponent->AcceptInsertStoneQuest();
}

void AQuestNpcBase::BuildInteractionActions(APawn* Interactor, TArray<FInteractionActionType>& OutActions) const
{
	OutActions.Reset();

	if (!CanInteract(Interactor))
	{
		return;
	}

	for (const TObjectPtr<UInteractionActionDefinition>& ActionDef : InteractionActions)
	{
		if (!ActionDef || !ActionDef->ActionTag.IsValid())
		{
			continue;
		}

		FInteractionActionType ActionView{};
		ActionView.bIsEnabled = ActionDef->bEnabledByDefault; 
		ActionView.Definition = ActionDef;

		OutActions.Add(ActionView);
	}
}

bool AQuestNpcBase::ExecuteInteractionAction(APawn* Interactor, FGameplayTag ActionTag)
{
	if (!CanInteract(Interactor) || !ActionTag.IsValid())
	{
		return false;
	}

	// 1. Check if the action tag matches any of the defined interaction actions.
	const UInteractionActionDefinition* MatchedActionDef = nullptr;

	for (const TObjectPtr<UInteractionActionDefinition>& ActionDef : InteractionActions)
	{
		if (!ActionDef || !ActionDef->ActionTag.IsValid())
		{
			continue;
		}

		if (ActionDef->ActionTag.MatchesTagExact(ActionTag))
		{
			MatchedActionDef = ActionDef;
			break;
		}
	}

	if (!MatchedActionDef || !MatchedActionDef->bEnabledByDefault)
	{
		return false;
	}

	// 2. Execute the corresponding action based on the matched tag.
	if (ActionTag.MatchesTagExact(TagListInteraction::QuestAccept))
	{

		Interact(Interactor);
		return true;
	}

	UE_LOGFMT(LogProjectA, Warning, "{0} - Unsupported interaction action tag: {1}", FString(__FUNCTION__), ActionTag.ToString());
	return false;
}




#include "QuestInteractableActor.h"

#include "QuestObjectiveSourceComponent.h"

AQuestInteractableActor::AQuestInteractableActor()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	QuestObjectiveSourceComponent = CreateDefaultSubobject<UQuestObjectiveSourceComponent>(TEXT("QuestObjectiveSourceComponent"));
}

bool AQuestInteractableActor::CanInteract(APawn* Interactor) const
{
	return QuestObjectiveSourceComponent != nullptr;
}


void AQuestInteractableActor::BuildInteractionActions(APawn* Interactor, TArray<FInteractionActionType>& OutActions) const
{
	if (!QuestObjectiveSourceComponent)
	{
		OutActions.Reset();
		return;
	}

	QuestObjectiveSourceComponent->BuildActions(Interactor, OutActions);
}

bool AQuestInteractableActor::ExecuteInteractionAction(APawn* Interactor, FGameplayTag ActionTag)
{
	const bool bWasExecuted = QuestObjectiveSourceComponent && QuestObjectiveSourceComponent->TryExecute(Interactor, ActionTag);

	if (bWasExecuted)
	{
		OnQuestInteractionExecuted.Broadcast(Interactor);
	}

	return bWasExecuted;
}
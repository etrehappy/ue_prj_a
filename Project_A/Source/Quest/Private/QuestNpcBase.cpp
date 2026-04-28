#include "QuestNpcBase.h"
#include "QuestLogComponent.h"

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
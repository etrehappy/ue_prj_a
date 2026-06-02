


#include "CustomDialogueTypes.h"
#include "DialogueDefinition.h"
#include "QuestLogComponent.h"
#include "QuestManagerSubsystem.h"
#include "Interactable.h"

#include "ProjectALog.h"

namespace CustomDialogueTypesHelpers
{
	static UQuestManagerSubsystem* GetQMS(const FDialogueRuntimeContext& RuntimeContext)
	{
		UWorld* World = RuntimeContext.World;

		if (!World)
		{
			World = RuntimeContext.PlayerPawn->GetWorld();
		}

		if (!World)	{ return nullptr; }

		return World->GetSubsystem<UQuestManagerSubsystem>();		
	}
}


///////////////////////////////////////////////////////////////////////////////
/// 
/// Conditions
/// 
///////////////////////////////////////////////////////////////////////////////

bool UDialogueCondition::IsSatisfied(const FDialogueRuntimeContext& RuntimeContext) const
{
	return true;
}

bool UDialogueConditionQuestState::IsSatisfied(const FDialogueRuntimeContext& RuntimeContext) const
{
	if (QuestId.IsNone() || !RuntimeContext.PlayerPawn)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Invalid QuestId or PlayerPawn", FString(__FUNCTION__));
		return false;
	}

	const UQuestLogComponent* QuestLogComponent = RuntimeContext.PlayerPawn->FindComponentByClass<UQuestLogComponent>();
	if (!QuestLogComponent)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - PlayerPawn is missing QuestLogComponent", FString(__FUNCTION__));
		return false;
	}

	switch (RequiredState)
	{
	case EDialogueQuestStateRequirement::NotAccepted:
		return !QuestLogComponent->IsQuestAccepted(QuestId)
			&& !QuestLogComponent->IsQuestReadyToTurnIn(QuestId)
			&& !QuestLogComponent->IsQuestTurnedIn(QuestId);

	case EDialogueQuestStateRequirement::Accepted:
		return QuestLogComponent->IsQuestAccepted(QuestId);

	case EDialogueQuestStateRequirement::ReadyToTurnIn:
		return QuestLogComponent->IsQuestReadyToTurnIn(QuestId);

	case EDialogueQuestStateRequirement::TurnedIn:
		return QuestLogComponent->IsQuestTurnedIn(QuestId);

	default:
		UE_LOGFMT(LogProjectA, Warning, "{0} - Invalid RequiredState value: {1}", FString(__FUNCTION__), static_cast<uint8>(RequiredState));
		break;
	}

	return false;
}





///////////////////////////////////////////////////////////////////////////////
/// 
/// Effects
/// 
///////////////////////////////////////////////////////////////////////////////

void UDialogueEffectAcceptQuest::Apply(const FDialogueRuntimeContext& RuntimeContext) const
{
	if (QuestId.IsNone() || !RuntimeContext.PlayerPawn)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Invalid QuestId or PlayerPawn", FString(__FUNCTION__));
		return;
	}
	auto* QuestManagerSubsystem = CustomDialogueTypesHelpers::GetQMS(RuntimeContext);
	if (!QuestManagerSubsystem)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - QuestManagerSubsystem is missing", FString(__FUNCTION__));
		return;
	}

	QuestManagerSubsystem->TryAcceptQuest(RuntimeContext.PlayerPawn, QuestId);
}

void UDialogueEffectTurnInQuest::Apply(const FDialogueRuntimeContext& RuntimeContext) const
{
	if (QuestId.IsNone() || !RuntimeContext.PlayerPawn)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Invalid QuestId or PlayerPawn", FString(__FUNCTION__));
		return;
	}

	auto* QuestManagerSubsystem = CustomDialogueTypesHelpers::GetQMS(RuntimeContext);
	if (!QuestManagerSubsystem)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - QuestManagerSubsystem is missing", FString(__FUNCTION__));
		return;
	}

	QuestManagerSubsystem->TryTurnInQuest(RuntimeContext.PlayerPawn, QuestId);
}

void UDialogueEffectApplyInteractionAction::Apply(const FDialogueRuntimeContext& RuntimeContext) const
{
	if (!RuntimeContext.PlayerPawn || !RuntimeContext.NpcActor || !ActionTag.IsValid())
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Invalid RuntimeContext or ActionTag", FString(__FUNCTION__));
		return;
	}

	IInteractable* Interactable = Cast<IInteractable>(RuntimeContext.NpcActor);
	if (!Interactable)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - NpcActor does not implement IInteractable", FString(__FUNCTION__));
		return;
	}

	if (bCheckCanInteract && !Interactable->CanInteract(RuntimeContext.PlayerPawn))
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - CanInteract check failed for PlayerPawn and NpcActor", FString(__FUNCTION__));
		return;
	}

	Interactable->ExecuteInteractionAction(RuntimeContext.PlayerPawn, ActionTag);
}
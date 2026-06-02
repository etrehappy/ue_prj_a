


#include "DialogueWidget.h"

#include "GeneralHud.h"
#include "GameFramework/PlayerController.h"

void UDialogueWidget::ShowDialogueNode(const FDialogueNodeRuntime& Node)
{
	CurrentNode = Node;
	SetVisibility(ESlateVisibility::Visible);
	BP_OnDialogueNodeUpdated(CurrentNode);
	BP_OnDialogueVisibilityChanged(true);	
}

void UDialogueWidget::HideDialogue()
{
	CurrentNode = FDialogueNodeRuntime{};
	SetVisibility(ESlateVisibility::Collapsed);
	BP_OnDialogueVisibilityChanged(false);
}

void UDialogueWidget::SubmitChoice(FName ChoiceId)
{
	if (!CurrentNode.SessionId.IsValid() || ChoiceId.IsNone())
	{
		return;
	}

	APlayerController* PlayerController = GetOwningPlayer();
	AGeneralHud* GeneralHud = PlayerController ? Cast<AGeneralHud>(PlayerController->GetHUD()) : nullptr;
	if (!GeneralHud)
	{
		return;
	}

	GeneralHud->SubmitDialogueChoice(CurrentNode.SessionId, ChoiceId);
}

void UDialogueWidget::CloseDialogue()
{
	APlayerController* PlayerController = GetOwningPlayer();
	AGeneralHud* GeneralHud = PlayerController ? Cast<AGeneralHud>(PlayerController->GetHUD()) : nullptr;
	if (!GeneralHud)
	{
		return;
	}

	GeneralHud->CloseDialogue();
}
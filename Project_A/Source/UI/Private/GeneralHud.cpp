


#include "GeneralHud.h"

#include "GeneralWidget.h"
#include "Blueprint/UserWidget.h"
#include "InteractionComponent.h"
#include "PlayerDialogueComponent.h"
#include "InventoryComponent.h"

#include "ProjectALog.h"

void AGeneralHud::BeginPlay()
{
	Super::BeginPlay();

	CreateMainWidget();
}

bool AGeneralHud::CreateMainWidget()
{
	if (MainWidget)
	{
		UE_LOGFMT(LogProjectA, Log, "{0} - MainWidget already created", FString(__FUNCTION__));
		return true;
	}

	if (!GeneralWidgetClass)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - GeneralWidgetClass is not set", FString(__FUNCTION__));
		return false;
	}
		
	const auto PlayerController = GetOwningPlayerController();
	if (!PlayerController)
	{
		UE_LOGFMT(LogProjectA, Error, "{0} - PlayerController is nullptr", FString(__FUNCTION__));
		return false;
	}		

	MainWidget = CreateWidget<UGeneralWidget>(PlayerController, GeneralWidgetClass);
	if (!MainWidget)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Failed to create MainWidget", FString(__FUNCTION__));
		return false;
	}

	MainWidget->AddToViewport();

	return true;
}

void AGeneralHud::ToggleInventory()
{
	if (!MainWidget)
	{
		if (!CreateMainWidget())
		{
			UE_LOGFMT(LogProjectA, Log, "{0} - MainWidget is not available", FString(__FUNCTION__));
			return;
		}
	}

	MainWidget->ToggleInventoryVisibility();
}

void AGeneralHud::InitialiseMainWidgetForPawn()
{
	if (!MainWidget)
	{
		if (!CreateMainWidget())
		{
			UE_LOGFMT(LogProjectA, Log, "{0} - MainWidget is not available", FString(__FUNCTION__));
			return;
		}
	}
		
	MainWidget->EnsureHealthInitialised();
	MainWidget->EnsureQuestInitialised();
	MainWidget->EnsureInventoryInitialised();
	MainWidget->EnsureEquippedItemInitialised();

	RebindPawnInventoryUiDelegates();	
}

void AGeneralHud::UpdatePartyMembers(const TArray<APawn*>& PartyPawns)
{
	if (!MainWidget)
	{
		if (!CreateMainWidget())
		{
			return;
		}
	}

	MainWidget->UpdatePartyMembers(PartyPawns);
}

void AGeneralHud::ToggleQuestJournal()
{
	if (!MainWidget)
	{
		if (!CreateMainWidget())
		{
			return;
		}
	}

	MainWidget->ToggleQuestJournalVisibility();
}

void AGeneralHud::ToggleMainMenu()
{
	if (!MainWidget)
	{
		if (!CreateMainWidget())
		{
			return;
		}
	}

	MainWidget->ToggleMainMenuVisibility();
}

void AGeneralHud::SubmitDialogueChoice(const FGuid SessionId, FName ChoiceId)
{
	APlayerController* PlayerController = GetOwningPlayerController();
	APawn* Pawn = PlayerController ? PlayerController->GetPawn() : nullptr;
	UPlayerDialogueComponent* DialogueComponent = Pawn ? Pawn->FindComponentByClass<UPlayerDialogueComponent>() : nullptr;
	if (!DialogueComponent)
	{
		return;
	}

	DialogueComponent->SubmitDialogueChoice(SessionId, ChoiceId);
}

void AGeneralHud::CloseDialogue()
{
	APlayerController* PlayerController = GetOwningPlayerController();
	APawn* Pawn = PlayerController ? PlayerController->GetPawn() : nullptr;
	UPlayerDialogueComponent* DialogueComponent = Pawn ? Pawn->FindComponentByClass<UPlayerDialogueComponent>() : nullptr;
	if (!DialogueComponent)
	{
		return;
	}

	DialogueComponent->CloseDialogue();
}

void AGeneralHud::ShowDialogueNode_Implementation(const FDialogueNodeRuntime& Node)
{
	if (!MainWidget)
	{
		if (!CreateMainWidget())
		{
			return;
		}
	}

	MainWidget->ShowDialogueNode(Node);
}

void AGeneralHud::HideDialogue_Implementation()
{
	if (!MainWidget)
	{
		return;
	}

	MainWidget->HideDialogue();
}

void AGeneralHud::SetDeathMenuVisibleState(bool bVisible)
{
	bDeathMenuVisible = bVisible;
}

void AGeneralHud::RebindPawnInventoryUiDelegates()
{
	if (!MainWidget)
	{
		return;
	}
		
	if (BoundInventoryComponent.IsValid())
	{
		BoundInventoryComponent->OnOpenTargetInventoryRequested.RemoveDynamic(MainWidget, &UGeneralWidget::OpenTargetInventory);
		BoundInventoryComponent->OnCloseTargetInventoryRequested.RemoveDynamic(MainWidget, &UGeneralWidget::CloseTargetInventory);
		BoundInventoryComponent = nullptr;
	}

	APlayerController* PlayerController = GetOwningPlayerController();
	APawn* Pawn = PlayerController ? PlayerController->GetPawn() : nullptr;
	if (!Pawn)
	{
		MainWidget->CloseTargetInventory();
		return;
	}

	UInventoryComponent* InventoryComponent = Pawn->FindComponentByClass<UInventoryComponent>();
	if (!InventoryComponent)
	{
		MainWidget->CloseTargetInventory();
		return;
	}

	InventoryComponent->OnOpenTargetInventoryRequested.AddUniqueDynamic(MainWidget, &UGeneralWidget::OpenTargetInventory);
	InventoryComponent->OnCloseTargetInventoryRequested.AddUniqueDynamic(MainWidget, &UGeneralWidget::CloseTargetInventory);

	BoundInventoryComponent = InventoryComponent;
}
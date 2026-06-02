#include "GeneralWidget.h"

#include "Blueprint/UserWidget.h"
#include "Components/Widget.h"
#include "DialogueWidget.h"
#include "EquippedItemWidget.h"
#include "GameFramework/Pawn.h"
#include "GeneralHud.h"
#include "HealthComponent.h"
#include "HealthWidget.h"
#include "InteractionInventoryProvider.h"
#include "InventoryComponent.h"
#include "InventoryWidget.h"
#include "Kismet/KismetSystemLibrary.h"
#include "QuestJournalWidget.h"
#include "QuestLogComponent.h"

#include "ProjectALog.h"

void UGeneralWidget::EnsureInventoryInitialised()
{
	APawn* OwningPawn = GetOwningPlayerPawn();
	if (!OwningPawn)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - OwningPawn is nullptr", FString(__FUNCTION__));
		bInventoryInitialised = false;
		CachedInventoryPawn = nullptr;
		return;
	}

	if (bInventoryInitialised && CachedInventoryPawn.Get() == OwningPawn)
	{
		return;
	}

	if (!InventoryWidget)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - InventoryWidget is nullptr", FString(__FUNCTION__));
		bInventoryInitialised = false;
		return;
	}

	UInventoryComponent* InventoryComponent = OwningPawn->GetComponentByClass<UInventoryComponent>();
	if (!InventoryComponent || !InventoryComponent->GetInventory())
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - InventoryComponent/Inventory not found on pawn {1}", FString(__FUNCTION__), *OwningPawn->GetName());
		bInventoryInitialised = false;
		return;
	}

	InventoryWidget->InitialiseWithInventory(InventoryComponent->GetInventory());
	bInventoryInitialised = true;
	CachedInventoryPawn = OwningPawn;
}

void UGeneralWidget::EnsureEquippedItemInitialised()
{
	APawn* OwningPawn = GetOwningPlayerPawn();
	if (!OwningPawn)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - OwningPawn is nullptr", FString(__FUNCTION__));
		bWeaponInitialised = false;
		CachedWeaponPawn = nullptr;
		return;
	}

	if (bWeaponInitialised && CachedWeaponPawn.Get() == OwningPawn)
	{
		return;
	}

	if (!EquippedItemWidget)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - EquippedItemWidget is nullptr", FString(__FUNCTION__));
		bWeaponInitialised = false;
		return;
	}

	UInventoryComponent* InventoryComponent = OwningPawn->GetComponentByClass<UInventoryComponent>();
	if (!InventoryComponent)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - InventoryComponent not found on owning pawn {1}", FString(__FUNCTION__), *OwningPawn->GetName());
		bWeaponInitialised = false;
		return;
	}

	EquippedItemWidget->InitialiseWithInventory(InventoryComponent);
	bWeaponInitialised = true;
	CachedWeaponPawn = OwningPawn;
}

void UGeneralWidget::EnsureHealthInitialised()
{
	APawn* OwningPawn = GetOwningPlayerPawn();
	if (!OwningPawn)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - OwningPawn is nullptr", FString(__FUNCTION__));
		return;
	}

	if (bHealthInitialised && CachedHealthPawn.Get() == OwningPawn)
	{
		UE_LOGFMT(LogProjectA, Log, "{0} - Health already initialised for this pawn {1}", FString(__FUNCTION__), *OwningPawn->GetName());
		return;
	}

	if (!HealthWidget)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - HealthWidget is nullptr", FString(__FUNCTION__));
		return;
	}

	UHealthComponent* HealthComponent = OwningPawn->GetComponentByClass<UHealthComponent>();
	if (!HealthComponent)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - HealthComponent not found on owning pawn {1}", FString(__FUNCTION__), *OwningPawn->GetName());
		return;
	}

	HealthWidget->InitialiseWithHealthComponent(HealthComponent);
	bHealthInitialised = true;
	CachedHealthPawn = OwningPawn;
}

void UGeneralWidget::ToggleInventoryVisibility()
{
	if (!InventoryWidget)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - InventoryWidget is nullptr", FString(__FUNCTION__));
		return;
	}

	APlayerController* OwningPlayerController = GetOwningPlayer();
	if (!OwningPlayerController)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - OwningPlayerController is nullptr", FString(__FUNCTION__));
		return;
	}

	const bool bIsVisible = (InventoryWidget->GetVisibility() == ESlateVisibility::Visible);

	if (!bIsVisible)
	{
		EnsureInventoryInitialised();
		EnsureEquippedItemInitialised();
		InventoryWidget->SetVisibility(ESlateVisibility::Visible);
		EquippedItemWidget->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		InventoryWidget->SetVisibility(ESlateVisibility::Collapsed);
		EquippedItemWidget->SetVisibility(ESlateVisibility::Collapsed);
	}

	ApplyInputModeFromUiState();
}


void UGeneralWidget::EnsureQuestInitialised()
{
	APawn* OwningPawn = GetOwningPlayerPawn();
	if (!OwningPawn) { return; }

	if (bQuestInitialised && CachedQuestPawn.Get() == OwningPawn)
	{
		return;
	}

	if (!QuestJournalWidget) { return; }

	UQuestLogComponent* QuestLogComponent = OwningPawn->FindComponentByClass<UQuestLogComponent>();
	if (!QuestLogComponent)	{ return; }

	QuestJournalWidget->InitialiseWithQuestLog(QuestLogComponent);
	bQuestInitialised = true;
	CachedQuestPawn = OwningPawn;
}

void UGeneralWidget::ToggleQuestJournalVisibility()
{
	if (!QuestJournalWidget) { return; }

	EnsureQuestInitialised();

	const bool bIsVisible = (QuestJournalWidget->GetVisibility() == ESlateVisibility::Visible);
	QuestJournalWidget->SetVisibility(bIsVisible ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);

	ApplyInputModeFromUiState();
}

void UGeneralWidget::ShowDialogueNode(const FDialogueNodeRuntime& Node)
{
	if (!DialogueWidget)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - DialogueWidget is nullptr", FString(__FUNCTION__));
		return;
	}

	DialogueWidget->ShowDialogueNode(Node);
	ApplyInputModeFromUiState();
}

void UGeneralWidget::HideDialogue()
{
	if (!DialogueWidget)
	{
		return;
	}

	DialogueWidget->HideDialogue();
	ApplyInputModeFromUiState();
}

void UGeneralWidget::OpenTargetInventory(AActor* TargetActor)
{
	UE_LOGFMT(LogProjectA, Warning, "{0} - Attempting to open target inventory for actor {1}", FString(__FUNCTION__), TargetActor ? *TargetActor->GetName() : TEXT("nullptr"));

	if (!TargetActor || !TargetInventoryWidget)	{ return; }

	IInteractionInventoryProvider* InventoryProvider = Cast<IInteractionInventoryProvider>(TargetActor);
	if (!InventoryProvider)	{ return; }

	UInventoryComponent* TargetInventoryComponent = InventoryProvider->GetInventoryComponent();
	if (!TargetInventoryComponent || !TargetInventoryComponent->GetInventory())
	{
		return;
	}

	EnsureInventoryInitialised();	

	TargetInventoryWidget->InitialiseWithInventory(TargetInventoryComponent->GetInventory());

	InventoryWidget->SetVisibility(ESlateVisibility::Visible);	
	TargetInventoryWidget->SetVisibility(ESlateVisibility::Visible);
	CurrentInventoryTarget = TargetActor;

	ApplyInputModeFromUiState();

	UE_LOGFMT(LogProjectA, Warning, "{0} - Opened target inventory for actor {1}", FString(__FUNCTION__), *TargetActor->GetName());
}

void UGeneralWidget::CloseTargetInventory()
{
	if (!TargetInventoryWidget)	{ return; }

	TargetInventoryWidget->SetVisibility(ESlateVisibility::Collapsed);
	InventoryWidget->SetVisibility(ESlateVisibility::Collapsed);
	CurrentInventoryTarget = nullptr;

	ApplyInputModeFromUiState();
}

void UGeneralWidget::ApplyInputModeFromUiState()
{
	APlayerController* PlayerController = GetOwningPlayer();
	if (!PlayerController)
	{
		return;
	}

	const bool bMainMenuVisible = MainMenuWidget && MainMenuWidget->GetVisibility() == ESlateVisibility::Visible;
	const bool bTargetInventoryVisible = TargetInventoryWidget && TargetInventoryWidget->GetVisibility() == ESlateVisibility::Visible;
	const bool bInventoryVisible = InventoryWidget && InventoryWidget->GetVisibility() == ESlateVisibility::Visible;
	const bool bQuestVisible = QuestJournalWidget && QuestJournalWidget->GetVisibility() == ESlateVisibility::Visible;
	const bool bDialogueVisible = DialogueWidget && DialogueWidget->GetVisibility() == ESlateVisibility::Visible;
	const bool bUiConsumesInput = bTargetInventoryVisible || bDialogueVisible;


	APawn* Pawn = PlayerController->GetPawn();
	if (Pawn)
	{
		if (bUiConsumesInput)
		{
			Pawn->DisableInput(PlayerController);			
			PlayerController->SetIgnoreMoveInput(false);
			PlayerController->SetIgnoreLookInput(false);
		}
		else if (bMainMenuVisible)
		{
			Pawn->EnableInput(PlayerController);
			PlayerController->SetIgnoreMoveInput(true);
			PlayerController->SetIgnoreLookInput(true);
		}
		else
		{			
			Pawn->EnableInput(PlayerController);
			PlayerController->SetIgnoreMoveInput(false);
			PlayerController->SetIgnoreLookInput(false);
		}
	}

	if (bUiConsumesInput || bMainMenuVisible || bQuestVisible || bInventoryVisible)
	{
		FInputModeGameAndUI InputMode;

		if (bMainMenuVisible)
		{
			InputMode.SetWidgetToFocus(MainMenuWidget->TakeWidget());
		}
		else if (bTargetInventoryVisible)
		{
			InputMode.SetWidgetToFocus(TargetInventoryWidget->TakeWidget());
		}
		else if (bInventoryVisible)
		{
			InputMode.SetWidgetToFocus(InventoryWidget->TakeWidget());
		}
		else if (bQuestVisible)
		{
			InputMode.SetWidgetToFocus(QuestJournalWidget->TakeWidget());
		}
		else if (bDialogueVisible)
		{
			InputMode.SetWidgetToFocus(DialogueWidget->TakeWidget());
		}

		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		InputMode.SetHideCursorDuringCapture(false);

		PlayerController->SetInputMode(InputMode);
		PlayerController->SetShowMouseCursor(true);
		PlayerController->bEnableClickEvents = true;
		PlayerController->bEnableMouseOverEvents = true;
	}
	else
	{
		FInputModeGameOnly GameInputMode;
		PlayerController->SetInputMode(GameInputMode);
		PlayerController->SetShowMouseCursor(false);
		PlayerController->bEnableClickEvents = false;
		PlayerController->bEnableMouseOverEvents = false;
	}
}

bool UGeneralWidget::IsMainMenuVisible() const
{
	return MainMenuWidget && MainMenuWidget->GetVisibility() == ESlateVisibility::Visible;
}

void UGeneralWidget::CloseUiForMainMenuOpen()
{
	if (DialogueWidget && DialogueWidget->GetVisibility() == ESlateVisibility::Visible)
	{
		DialogueWidget->HideDialogue();
	}

	if (TargetInventoryWidget && TargetInventoryWidget->GetVisibility() == ESlateVisibility::Visible)
	{
		TargetInventoryWidget->SetVisibility(ESlateVisibility::Collapsed);
		CurrentInventoryTarget = nullptr;
	}

	if (InventoryWidget && InventoryWidget->GetVisibility() == ESlateVisibility::Visible)
	{
		InventoryWidget->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (EquippedItemWidget && EquippedItemWidget->GetVisibility() == ESlateVisibility::Visible)
	{
		EquippedItemWidget->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (QuestJournalWidget && QuestJournalWidget->GetVisibility() == ESlateVisibility::Visible)
	{
		QuestJournalWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}


void UGeneralWidget::OpenInventoryFromMainMenu()
{
	if (IsMainMenuVisible())
	{
		MainMenuWidget->SetVisibility(ESlateVisibility::Collapsed);
	}

	EnsureInventoryInitialised();
	EnsureEquippedItemInitialised();

	if (InventoryWidget)
	{
		InventoryWidget->SetVisibility(ESlateVisibility::Visible);
	}

	if (EquippedItemWidget)
	{
		EquippedItemWidget->SetVisibility(ESlateVisibility::Visible);
	}

	ApplyInputModeFromUiState();
}

void UGeneralWidget::OpenQuestJournalFromMainMenu()
{
	if (IsMainMenuVisible())
	{
		MainMenuWidget->SetVisibility(ESlateVisibility::Collapsed);
	}

	EnsureQuestInitialised();

	if (QuestJournalWidget)
	{
		QuestJournalWidget->SetVisibility(ESlateVisibility::Visible);
	}

	ApplyInputModeFromUiState();
}

void UGeneralWidget::QuitGameFromMainMenu()
{
	APlayerController* PlayerController = GetOwningPlayer();
	if (!PlayerController)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - PlayerController is nullptr", FString(__FUNCTION__));
		return;
	}

	UKismetSystemLibrary::QuitGame(this, PlayerController, EQuitPreference::Quit, false);
}


bool UGeneralWidget::IsDeathMenuVisible() const
{
	const APlayerController* PlayerController = GetOwningPlayer();
	const AGeneralHud* HUD = PlayerController ? Cast<AGeneralHud>(PlayerController->GetHUD()) : nullptr;
	return HUD && HUD->IsDeathMenuVisibleState();
}

void UGeneralWidget::CloseConflictingUiForMainMenu()
{
	if (DialogueWidget && DialogueWidget->GetVisibility() == ESlateVisibility::Visible)
	{
		DialogueWidget->HideDialogue();
	}

	if (TargetInventoryWidget && TargetInventoryWidget->GetVisibility() == ESlateVisibility::Visible)
	{
		TargetInventoryWidget->SetVisibility(ESlateVisibility::Collapsed);
		CurrentInventoryTarget = nullptr;
	}

	if (InventoryWidget && InventoryWidget->GetVisibility() == ESlateVisibility::Visible)
	{
		InventoryWidget->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (EquippedItemWidget && EquippedItemWidget->GetVisibility() == ESlateVisibility::Visible)
	{
		EquippedItemWidget->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (QuestJournalWidget && QuestJournalWidget->GetVisibility() == ESlateVisibility::Visible)
	{
		QuestJournalWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UGeneralWidget::ToggleMainMenuVisibility()
{
	if (!MainMenuWidget)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - MainMenuWidget is nullptr", FString(__FUNCTION__));
		return;
	}
		
	if (IsDeathMenuVisible())
	{
		UE_LOGFMT(LogProjectA, Log, "{0} - DeathMenu is visible, skip", FString(__FUNCTION__));
		return;
	}
		
	if (IsMainMenuVisible())
	{
		MainMenuWidget->SetVisibility(ESlateVisibility::Collapsed);
		ApplyInputModeFromUiState();
		return;
	}
		
	CloseConflictingUiForMainMenu();
	MainMenuWidget->SetVisibility(ESlateVisibility::Visible);

	ApplyInputModeFromUiState();
}

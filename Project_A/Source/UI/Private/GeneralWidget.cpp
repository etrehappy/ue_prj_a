


#include "GeneralWidget.h"

#include "Blueprint/UserWidget.h"
#include "Components/Widget.h"
#include "GameFramework/Pawn.h"
#include "InventoryComponent.h"
#include "InventoryWidget.h"
#include "EquippedItemWidget.h"

#include "ProjectALog.h"

void UGeneralWidget::EnsureInventoryInitialised()
{
	if (bInventoryInitialised) { return; }

	if (!InventoryWidget)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - InventoryWidget is nullptr", FString(__FUNCTION__));
		return;
	}

	APawn* OwningPawn = GetOwningPlayerPawn();
	if (!OwningPawn)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - OwningPawn is nullptr", FString(__FUNCTION__));
		return;
	}

	UInventoryComponent* InventoryComponent = OwningPawn->GetComponentByClass<UInventoryComponent>();
	if (!InventoryComponent)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - InventoryComponent not found on owning pawn {1}", FString(__FUNCTION__), *OwningPawn->GetName());
		return;
	}

	InventoryWidget->InitialiseWithInventory(InventoryComponent->GetInventory());
	bInventoryInitialised = true;
}

void UGeneralWidget::EnsureEquippedItemInitialised()
{
	if (bWeaponInitialised) { return; }

	if (!EquippedItemWidget)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - EquippedItemWidget is nullptr", FString(__FUNCTION__));
		return;
	}

	APawn* OwningPawn = GetOwningPlayerPawn();
	if (!OwningPawn)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - OwningPawn is nullptr", FString(__FUNCTION__));
		return;
	}

	UInventoryComponent* InventoryComponent = OwningPawn->GetComponentByClass<UInventoryComponent>();
	if (!InventoryComponent)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - InventoryComponent not found on owning pawn {1}", FString(__FUNCTION__), *OwningPawn->GetName());
		return;
	}

	EquippedItemWidget->InitialiseWithInventory(InventoryComponent);
	bWeaponInitialised = true;
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

	
		OwningPlayerController->SetShowMouseCursor(true);
		FInputModeGameAndUI InputMode;
		OwningPlayerController->SetInputMode(InputMode);		
	}
	else
	{
		InventoryWidget->SetVisibility(ESlateVisibility::Collapsed);
		EquippedItemWidget->SetVisibility(ESlateVisibility::Collapsed);

		OwningPlayerController->SetShowMouseCursor(false);
		FInputModeGameOnly GameInputMode;
		OwningPlayerController->SetInputMode(GameInputMode);	
	}
}



#include "InventoryWidget.h"

#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "Inventory.h"
#include "InventorySlotWidget.h"

#include "ProjectALog.h"


void UInventoryWidget::InitialiseWithInventory(UInventory* InInventory)
{
	if (!InInventory)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - InInventoryComponent is null", FString(__FUNCTION__));
		return;
	}

	Inventory = InInventory;
	Inventory->OnInventoryChanged.AddDynamic(this, &UInventoryWidget::HandleInventoryChanged);

	RebuildSlots();
}

void UInventoryWidget::HandleInventoryChanged()
{
	RebuildSlots();
}

void UInventoryWidget::RebuildSlots()
{
	// Guard checks
	if (!SlotWidgetClass)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - SlotWidgetClass is not set", FString(__FUNCTION__));
		return;
	}

	if (!SlotsGrid)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - SlotsGrid is not set", FString(__FUNCTION__));
		return;
	}

	UInventory* InventoryPtr = Inventory.Get();
	if (!InventoryPtr)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Inventory is null", FString(__FUNCTION__));
		return;
	}

	const int32 TotalSlots = InventoryPtr->GetSize();
	if (TotalSlots <= 0)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Inventory has no slots", FString(__FUNCTION__));
		return;
	}

	//Prepare
	SlotsGrid->ClearChildren(); // Clear existing slot widgets before rebuilding
	const auto& InventorySlots = InventoryPtr->GetSlots();


	// Build slots on the screen
	for (int32 Index = 0; Index < TotalSlots; ++Index)
	{
		UInventorySlotWidget* SlotWidget = CreateWidget<UInventorySlotWidget>(GetOwningPlayer(), SlotWidgetClass);

		if (!SlotWidget)
		{
			UE_LOGFMT(LogProjectA, Warning, "{0} - Failed to create SlotWidget at index {1}", FString(__FUNCTION__), Index);
			continue;
		}
		SlotWidget->InitialiseSlot(InventoryPtr, Index);

		// Set slot data
		const bool bHasData = InventorySlots.IsValidIndex(Index);
		const auto SlotData = bHasData ? InventorySlots[Index] : FInventorySlot{};

		SlotWidget->SetSlotData(SlotData);

		// Add to grid
		int32 Row{}, Column{};
		if (Columns > 0)
		{
			Row = Index / Columns;
			Column = Index % Columns;
		}
		else
		{
			Row = 0;
			Column = Index;
		}

		if (UUniformGridSlot* GridSlot = SlotsGrid->AddChildToUniformGrid(SlotWidget, Row, Column))
		{
			GridSlot->SetHorizontalAlignment(HAlign_Fill);
			GridSlot->SetVerticalAlignment(VAlign_Fill);
		}
	}
}
#include "InventorySlotWidget.h"

#include "Blueprint/DragDropOperation.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Image.h"
#include "Engine/Texture2D.h"
#include "Inventory.h" 
#include "InventoryItem.h"

#include "ProjectALog.h"

void UInventorySlotWidget::InitialiseSlot(UInventory* InInventory, int32 InSlotIndex)
{
	if (!InInventory)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - InInventory is null", FString(__FUNCTION__));
		return;
	}

	Inventory = InInventory;
	SlotIndex = InSlotIndex;

	BindInventory(InInventory);
	HandleInventoryChanged();
}

void UInventorySlotWidget::BindInventory(UInventory* InInventory)
{
	if (LastBoundInventory.IsValid() && LastBoundInventory != InInventory)
	{
		LastBoundInventory->OnInventoryChanged.RemoveAll(this);
	}

	if (InInventory)
	{
		InInventory->OnInventoryChanged.RemoveAll(this);
		InInventory->OnInventoryChanged.AddUniqueDynamic(this, &UInventorySlotWidget::HandleInventoryChanged);
	}

	LastBoundInventory = InInventory;
}

void UInventorySlotWidget::HandleInventoryChanged()
{
	if (!Inventory.IsValid())
	{
		ItemImage->SetBrushFromTexture(nullptr);
		ItemImage->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}

	const TArray<FInventorySlot>& Slots = Inventory->GetSlots();
	if (Slots.IsValidIndex(SlotIndex))
	{
		SetSlotData(Slots[SlotIndex]);
		return;
	}

	ItemImage->SetBrushFromTexture(nullptr);
	ItemImage->SetVisibility(ESlateVisibility::Collapsed);
}

void UInventorySlotWidget::SetSlotData(const FInventorySlot& InSlot)
{
	if (InSlot.Item && InSlot.Item->HasValidData())
	{
		ItemImage->SetBrushFromTexture(InSlot.Item->Definition->Icon.Get(), true);				
	}
	else
	{
		//UE_LOGFMT(LogProjectA, Log, "{0} - No valid slot data found, clearing the image", FString(__FUNCTION__));
		ItemImage->SetBrushFromTexture(nullptr);
	}

	ItemImage->SetVisibility(ESlateVisibility::Visible);
}

FReply UInventorySlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	return UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton).NativeReply;
}

void UInventorySlotWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InPointerEvent, UDragDropOperation*& OutOperation)
{
	OutOperation = nullptr;

	if (!Inventory.IsValid()) { return;	}

	const TArray<FInventorySlot>& Slots = Inventory->GetSlots();
	if (!Slots.IsValidIndex(SlotIndex) || !Slots[SlotIndex].Item || !Slots[SlotIndex].Item->HasValidData())
	{
		return;
	}

	// Create operation 
	UDragDropOperation* DragDropOperation = NewObject<UDragDropOperation>(this);
	DragDropOperation->Payload = this;
	DragDropOperation->Pivot = EDragPivot::CenterCenter;
	DragDropOperation->Offset = FVector2D(0.f, 0.f);

	// A simple visual for the drag
	UImage* DragImage = NewObject<UImage>(this);
	UTexture2D* Icon = Slots[SlotIndex].Item->Definition->Icon.Get();
	if (Icon)
	{
		DragImage->SetBrushFromTexture(Icon, false);
		DragImage->SetDesiredSizeOverride(FVector2D(64.f, 64.f));
	}
	DragDropOperation->DefaultDragVisual = DragImage;

	// Return the operation
	OutOperation = DragDropOperation;
}

bool UInventorySlotWidget::NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	if (!InOperation || !InOperation->Payload)
	{
		UE_LOGFMT(LogProjectA, Log, "{0} - Invalid drag operation or payload", FString(__FUNCTION__));
		return false;
	}

	if (!Cast<UInventorySlotWidget>(InOperation->Payload))
	{
		UE_LOGFMT(LogProjectA, Log, "{0} - Drag payload is not an inventory slot or equipped item slot widget", FString(__FUNCTION__));
		return false;
	}

	return true;
}

bool UInventorySlotWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	// Validate
	if (!InOperation || !InOperation->Payload)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Invalid drop operation or payload", FString(__FUNCTION__));
		return false;
	}

	UInventorySlotWidget* SourceSlot = Cast<UInventorySlotWidget>(InOperation->Payload);
	if (!SourceSlot)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Drop payload is not an inventory slot widget", FString(__FUNCTION__));
		return false;
	}
	
	if (!SourceSlot->Inventory.IsValid() || !Inventory.IsValid())
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Invalid inventories on drop", FString(__FUNCTION__));
		return false;
	}
	
	// Preparation
	UInventory* SourceInventory = SourceSlot->Inventory.Get(); // Other inventory with another slot-widget	
	UInventory* TargetInventory = Inventory.Get(); // The inventory owning this slot-widget
	int32 SourceSlotIndex = SourceSlot->SlotIndex;
	int32 TargetSlotIndex = SlotIndex;

	bool bIsSameInventory = (SourceInventory && TargetInventory && SourceInventory == TargetInventory);
	bool bIsDifferentInventory = (SourceInventory && TargetInventory && SourceInventory != TargetInventory);

	// Handle the drop
	if (bIsSameInventory)
	{
		SourceInventory->RelocateItemInInventory(SourceSlotIndex, TargetSlotIndex);
		return true;
	}
	else if (bIsDifferentInventory)
	{
		SourceInventory->MoveItemToOtherInventory(SourceSlotIndex, TargetInventory, TargetSlotIndex);
		return true;
	}
	else
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Unhandled drop case", FString(__FUNCTION__));
	}

	return false;
}


//void UInventorySlotWidget::NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* Operation)
//{
//	// Clean up any resources or visuals created for the drag operation if necessary.
//}

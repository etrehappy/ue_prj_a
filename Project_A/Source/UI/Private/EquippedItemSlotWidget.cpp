


#include "EquippedItemSlotWidget.h"

#include "Blueprint/DragDropOperation.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Engine/Texture2D.h"
#include "Inventory.h"
#include "InventoryItem.h"

#include "ProjectALog.h"


//void UEquippedItemSlotWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InPointerEvent, UDragDropOperation*& OutOperation)
//{
//	OutOperation = nullptr;
//
//	// Validate
//	if (!UInventorySlotWidget::Inventory.IsValid())	{ return; }
//
//	const UInventory* InventoryPtr = UInventorySlotWidget::Inventory.Get();
//	if (!InventoryPtr) { return; }
//
//	const UInventoryItem* EquippedItem = InventoryPtr->FindItemBySlot(UInventorySlotWidget::SlotIndex);
//	if (!EquippedItem || !EquippedItem->HasValidData())	{ return; }
//
//	// Create operation 
//	UDragDropOperation* DragDropOperation = NewObject<UDragDropOperation>(this);
//	DragDropOperation->Payload = this;
//	DragDropOperation->Pivot = EDragPivot::CenterCenter;
//	DragDropOperation->Offset = FVector2D::ZeroVector;
//
//	// A simple visual for the drag
//	UImage* DragImage = NewObject<UImage>(this);
//	UTexture2D* Icon = EquippedItem->Definition->Icon.Get();
//	if (Icon)
//	{
//		DragImage->SetBrushFromTexture(Icon, false);
//		DragImage->SetDesiredSizeOverride(FVector2D(64.f, 64.f));
//	}	
//	DragDropOperation->DefaultDragVisual = DragImage;
//
//	// Return the operation
//	OutOperation = DragDropOperation;
//}




/*****************************************************************//**
 * \file   EquippedItemSlotWidget.h
 * \brief  Widget representing a single equipment slot (e.g. weapon) in the UI.
 * 
 * \date   February 2026
 *********************************************************************/



#pragma once

#include "CoreMinimal.h"
#include "BaseWidget.h"
#include "InventorySlot.h"
#include "InventorySlotWidget.h"
#include "InventoryComponent.h"

#include "EquippedItemSlotWidget.generated.h"

class UImage;
class UTextBlock;

/**
 * @class UEquippedItemSlotWidget
 * @todo Restrict the types of items that can be equipped in this slot (e.g. only physical weapons).
 */
UCLASS()
class UI_API UEquippedItemSlotWidget : public UInventorySlotWidget
{
	GENERATED_BODY()

protected:

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> SlotLabel{};
	
	/**
	 * @brief The parent - UInventorySlotWidget - uses 'int32 SlotIndex' to identify the slot, 
	 * but for equipped items it is more intuitive to use EEquipmentSlot enum.
	 */
	UPROPERTY(BlueprintReadOnly)
	EEquipmentSlot EquipmentSlot{EEquipmentSlot::None};
};

/*virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InPointerEvent, UDragDropOperation*& OutOperation) override;*/
/*****************************************************************//**
 * \file   InventorySlotWidget.h
 * \brief  Widget representing an inventory slot in the UI.
 * 
 * \date   February 2026
 *********************************************************************/
#pragma once

#include "CoreMinimal.h"
#include "BaseWidget.h"
#include "InventorySlot.h"
#include "InventorySlotWidget.generated.h"

class UInventory;
class UImage;
class UDragDropOperation;

/**
 * @class UInventorySlotWidget
 * @brief Widget representing an inventory slot in the UI.
 */
UCLASS()
class UI_API UInventorySlotWidget : public UBaseWidget
{
	GENERATED_BODY()


						/* === C++ member functions === */
public:
	UInventorySlotWidget() = default;
	virtual ~UInventorySlotWidget() override = default;

	/**
	 * @brief It is required for drag and drop operations.
	 * 
	 * @param[in] InInventory The inventory to which this slot belongs.
	 * @param[in] InSlotIndex The index of the slot within the inventory.
	 */
	void InitialiseSlot(UInventory* InInventory, int32 InSlotIndex);

	// Overridden drag and drop events
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InPointerEvent, UDragDropOperation*& OutOperation) override;
	virtual bool NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	//

private:
	/**
	 * @brief A reaction to inventory changes is required.
	 * 
	 * @param[in] InInventory The inventory to bind to.
	 * @see HandleInventoryChanged
	 */
	void BindInventory(UInventory* InInventory);


						/* === C++ member variables === */
private:
	TWeakObjectPtr<UInventory> LastBoundInventory{};


						/* === Unreal Engine UFUNCTION === */
public:
	/**
	 * @brief Sets the data about InventoryItem.
	 *
	 * @param[in] InSlot The inventory slot data to set.
	 */
	UFUNCTION(BlueprintCallable)
	void SetSlotData(const FInventorySlot& InSlot);

	/**
	 * @brief Called when the context menu is requested for this inventory slot.
	 */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic)
	void OnContextMenuRequested(int32 InSlotIndex);

private:

	UFUNCTION()
	void HandleInventoryChanged();


						/* === Unreal Engine UPROPERTY === */
protected:

	/**
	 * @brief to drag and drop
	 */
	UPROPERTY(BlueprintReadOnly)
	int32 SlotIndex{};

	/**
	 * @brief UInventorySlotWidget should know about the inventory to drag and drop items between slots.
	 */
	UPROPERTY(BlueprintReadOnly)
	TWeakObjectPtr<UInventory> Inventory{};

	/**
	 * @brief
	 */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TSoftObjectPtr<UImage> ItemImage{};
};

//virtual void NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* Operation) override;

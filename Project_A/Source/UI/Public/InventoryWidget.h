/*****************************************************************//**
 * \file   InventoryWidget.h
 * \brief  Widget representing the player's inventory in the UI.
 * 
 * \date   February 2026
 *********************************************************************/
#pragma once

#include "CoreMinimal.h"
#include "BaseWidget.h"
#include "InventoryWidget.generated.h"

class UUniformGridPanel;
class UInventory;
class UInventorySlotWidget;

/**
 * @class UInventoryWidget
 * @brief It displays inventory slots and handles updates when the inventory changes.
 */
UCLASS()
class UI_API UInventoryWidget : public UBaseWidget
{
	GENERATED_BODY()

						/* === C++ member functions and variables === */
public:
	UInventoryWidget() = default;
	virtual ~UInventoryWidget() = default;

	/**
	 * @brief Initializes the widget with the given inventory component. Binds the widget to the inventory's change event.
	 *
	 * @param[in] Inventory The inventory component to associate with this widget.
	 */
	UFUNCTION()
	void InitialiseWithInventory(UInventory* Inventory);

private:
	/**
	 * @brief The inventory component that this widget is associated with.
	 */
	TWeakObjectPtr<UInventory> Inventory{};


						/* === Unreal Engine UFUNCTION === */
protected:
	/**
	 * @brief Rebuilds the inventory slots in the UI.
	 * 
	 */
	UFUNCTION(BlueprintCallable, BlueprintCosmetic)
	void RebuildSlots();

private:
	UFUNCTION()
	void HandleInventoryChanged();

						/* === Unreal Engine UPROPERTY === */
protected:
	/**
	 * @brief The grid panel that holds the inventory slots.
	 */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UUniformGridPanel> SlotsGrid{};

	/**
	 * @brief The class of the inventory slot widget to use.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TSubclassOf<UInventorySlotWidget> SlotWidgetClass{};

	/**
	 * @brief The number of columns in the inventory grid.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	int32 Columns{5};


};

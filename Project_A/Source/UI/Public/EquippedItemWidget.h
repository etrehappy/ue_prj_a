/*****************************************************************//**
 * \file   EquippedItemWidget.h
 * \brief  Widget representing equipped items.
 *
 * \date   February 2026
 *********************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "BaseWidget.h"

#include "EquippedItemWidget.generated.h"

class UInventoryComponent;
class UEquippedItemSlotWidget;

/**
 * @class UEquippedItemWidget
 * @brief Widget that displays equipped items (e.g. weapon).
 * @see UInventoryWidget
 */
UCLASS()
class UI_API UEquippedItemWidget : public UBaseWidget
{
	GENERATED_BODY()
public:
	UEquippedItemWidget() = default;
	virtual ~UEquippedItemWidget() override = default;

						/* === C++ member variables === */
private:
	/**
	 * @brief It is required for getting equipped items and for subscribing to equipment change events.
	 * @see HandleEquipmentChanged
	 */
	TWeakObjectPtr<UInventoryComponent> InventoryComponent{};


						/* === Unreal Engine UFUNCTION === */
public:
	/**
	 * @brief Binds the widget to the equipment change events.
	 *
	 * @param[in] InInventoryComponent The inventory component to initialise with.
	 */
	UFUNCTION()
	void InitialiseWithInventory(UInventoryComponent* InInventoryComponent);

private:
	/**
	 * @brief Updates the equipped item slots when the equipment changes.
	 * 
	 */
	UFUNCTION()
	void HandleEquipmentChanged();


						/* === Unreal Engine UPROPERTY === */
protected:
	/**
	 * @brief The widget representing the equipped weapon slot.
	 * @see UWeaponComponent::CurrentWeapon
	 */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UEquippedItemSlotWidget> WeaponSlot{};

	/**
	 * @brief The widget representing the equipped throwable item slot.
	 * @see UWeaponComponent::CurrentThrowableItem
	 */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UEquippedItemSlotWidget> ThrowableSlot{};
};
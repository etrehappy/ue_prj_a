/*****************************************************************//**
 * \file   GeneralWidget.h
 * \brief  First widget that is created when the player starts in a game-world.
 * 
 * \date   February 2026
 *********************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "BaseWidget.h"
#include "EquippedItemWidget.h"

#include "GeneralWidget.generated.h"

class UInventoryWidget;

/**
 * @class UGeneralWidget
 * @brief It contains inventory widget and other widgets that are common for all characters.
 */
UCLASS()
class UI_API UGeneralWidget : public UBaseWidget
{
	GENERATED_BODY()

						/* === C++ member functions and variables === */
protected:
	/**
	 * @brief Ensures that the inventory widget is initialised. If it is not, it initialises it.
	 */
	void EnsureInventoryInitialised();

	/**
	 * @brief Ensures that the equipped item widget is initialised. If it is not, it initialises it.
	 */
	void EnsureEquippedItemInitialised();

	bool bInventoryInitialised{false};
	bool bWeaponInitialised{false};

						/* === Unreal Engine UFUNCTION and UPROPERTY === */
public:
	/**
	 * @brief The InventoryWidget permanently exists in the widget hierarchy, but it is hidden by default. This function toggles its visibility.
	 */
	UFUNCTION(BlueprintCallable)
	void ToggleInventoryVisibility();
	
protected:

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UInventoryWidget> InventoryWidget{};

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UEquippedItemWidget> EquippedItemWidget{};

	

};

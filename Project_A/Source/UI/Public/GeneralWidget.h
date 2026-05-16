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
class UHealthWidget;

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
	bool bHealthInitialised{false};

private:
	/**
	 * @brief Cached reference to the pawn whose health is being displayed. 
	 * This is used to correctly unbind from the health change event when the pawn changes.
	 */
	TWeakObjectPtr<APawn> CachedHealthPawn{};

						/* === Unreal Engine UFUNCTION and UPROPERTY === */
public:
	/**
	 * @brief The InventoryWidget permanently exists in the widget hierarchy, but it is hidden by default. This function toggles its visibility.
	 */
	UFUNCTION(BlueprintCallable)
	void ToggleInventoryVisibility();

	/**
	 * @brief Ensures that the health widget is initialised. If it is not, it initialises it.
	 */
	UFUNCTION(BlueprintCallable)
	void EnsureHealthInitialised();

	UFUNCTION(BlueprintImplementableEvent, Category = "Party")
	void UpdatePartyMembers(const TArray<APawn*>& PartyPawns);
	
protected:

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UInventoryWidget> InventoryWidget{};

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UEquippedItemWidget> EquippedItemWidget{};

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UHealthWidget> HealthWidget{};

	

};

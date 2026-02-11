/*****************************************************************//**
 * \file   GeneralHud.h
 * \brief  HUD class that manages the general widget.
 * 
 * \date   February 2026
 *********************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"

#include "GeneralHud.generated.h"

class UGeneralWidget;

/**
 * @class AGeneralHud
 * @brief A HUD class that manages the general widget, which includes the inventory and other UI elements.
 */
UCLASS()
class UI_API AGeneralHud : public AHUD
{
	GENERATED_BODY()

						/* === C++ member functions and variables === */
protected:
	virtual void BeginPlay() override;

private:
	/**
	 * @brief Creates the main general widget and adds it to the viewport.
	 * 
	 * @see MainWidget
	 * @return true if the widget was successfully created and added, false otherwise.
	 */
	bool CreateMainWidget();
	

						/* === Unreal Engine UFUNCTION and UPROPERTY === */
public:
	/**
	 * @brief The InventoryWidget permanently exists in the widget hierarchy, but it is hidden by default. This function toggles its visibility.
	 */
	UFUNCTION(BlueprintCallable)
	void ToggleInventory();

protected:
	/**
	 * @brief The class type of the general widget to create and manage.
	 */
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGeneralWidget> GeneralWidgetClass{};

	/**
	 * @brief First widget that is created when the player starts in a game-world.
	 */
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UGeneralWidget> MainWidget{};



};

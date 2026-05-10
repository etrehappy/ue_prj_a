/*****************************************************************//**
 * \file   BaseHud.h
 * \brief  Default HUD class for the project. 
 * 
 * \date   May 2026
 *********************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "ProjectNetworkSettings.h"
#include "WorldServerView.h"
#include "Blueprint/UserWidget.h"
#include "CharacterSelectionView.h"

#include "BaseHud.generated.h"

/**
 * @class ABaseHud
 * @todo It should be the base class for all HUD. Same functionality should be implemented via interface. But now it's a simple way to use C++ and Blueprints together.
 */
UCLASS()
class UI_API ABaseHud : public AHUD
{
	GENERATED_BODY()
	
public:
	ABaseHud() = default;
	virtual ~ABaseHud() = default;

	void HideNetConnectionStatusWidget();

	/**
	 * @brief Executed when the connection to the server is successful
	 * @todo It does not any helpful functionality for now.
	 */
	void OnConnectedSuccessfully(EServerWorldType World);

private:
	void ShowMouse();
	void HideMouse();

public:
	UFUNCTION(BlueprintImplementableEvent)
	void ShowErrorScreen(const FString& ErrorMessage);	

	/**	 
	 * @brief It should implement a way to save a reference to the NetConnectionStatusWidget in the GameInstance.
	 * @see NetConnectionStatusWidget
	 * @todo BaseHud should be the base class for all HUD.
	 */
	UFUNCTION(BlueprintImplementableEvent)
	void SaveNetStatusWidgetReference(UUserWidget* Widget);

	/**
	 * @brief Used on the Client to update a server selection list.
	 * @todo Can be implemented through interface, but for now it's only used in the main menu.
	 */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void UpdateServerSelectionTable(const TArray<FWorldServerView>& Servers);

	/**
	 * @brief Used on the Client to show a character selection screen.
	 * @todo Shouldn't be in the BaseHud.
	 * @see BP HubHUD
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void ShowCharacterSelectionScreen();

	/**
	 * @brief Used on the Client to update a character list.
	 * @todo Can be implemented through interface, but for now it's only used in the main menu.
	 */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "UI")
	void UpdateCharacterSelectionTable(const TArray<UCharacterEntryObject*>& Characters);
		
protected:
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TObjectPtr<UUserWidget> NetConnectionStatusWidget;
};
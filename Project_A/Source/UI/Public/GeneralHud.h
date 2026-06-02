/*****************************************************************//**
 * \file   GeneralHud.h
 * \brief  HUD class that manages the general widget.
 * 
 * \date   February 2026
 *********************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "BaseHud.h"
#include "DialogueManagerSubsystem.h"

#include "GeneralHud.generated.h"

class UGeneralWidget;
struct FInteractionActionType;
class UInventoryComponent;

/**
 * @class AGeneralHud
 * @brief A HUD class that manages the general widget, which includes the inventory and other UI elements.
 */
UCLASS()
class UI_API AGeneralHud : public ABaseHud
{
	GENERATED_BODY()

						/* === C++ member functions and variables === */
public:
	void SetDeathMenuVisibleState(bool bVisible);
	bool IsDeathMenuVisibleState() const { return bDeathMenuVisible; }

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

	void RebindPawnInventoryUiDelegates();

	bool bDeathMenuVisible{false};
	TWeakObjectPtr<UInventoryComponent> BoundInventoryComponent{};

						/* === Unreal Engine UFUNCTION and UPROPERTY === */
public:
	/**
	 * @brief The InventoryWidget permanently exists in the widget hierarchy, but it is hidden by default. This function toggles its visibility.
	 */
	UFUNCTION(BlueprintCallable)
	void ToggleInventory();

	/**
	 * @brief Initializes the main widget for the pawn. This function should be called when the player possesses a new pawn to ensure that the HUD is correctly set up for the new character.
	 */
	UFUNCTION(BlueprintCallable)
	void InitialiseMainWidgetForPawn();

	UFUNCTION(BlueprintCallable)
	void UpdatePartyMembers(const TArray<APawn*>& PartyPawns);

	UFUNCTION(BlueprintImplementableEvent)
	void ShowInteractionMenu(AActor* TargetActor, const TArray<FInteractionActionType>& Actions);

	UFUNCTION(BlueprintCallable)
	void ToggleQuestJournal();

	UFUNCTION(BlueprintCallable)
	void ToggleMainMenu();

	UFUNCTION(BlueprintCallable)
	void SubmitDialogueChoice(const FGuid SessionId, FName ChoiceId);

	UFUNCTION(BlueprintCallable)
	void CloseDialogue();

	UFUNCTION(BlueprintNativeEvent)
	void ShowDialogueNode(const FDialogueNodeRuntime& Node);

	UFUNCTION(BlueprintNativeEvent)
	void HideDialogue();

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

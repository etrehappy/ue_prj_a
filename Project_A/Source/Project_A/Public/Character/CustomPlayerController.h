/*****************************************************************//**
 * \file   CustomPlayerController.h
 * \brief  Default PlayerController class for this project. 
 * 
 * \date   December 2025
 *********************************************************************/
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Core/GeneralGameMode.h"
#include "GameplayTagContainer.h"
#include "WorldServerView.h"
#include "CharacterSelectionView.h"

#include "CustomPlayerController.generated.h"

class UInputMappingContext;
class UDeathMenuWidget;
struct FInteractionActionType;

/**
 * @struct FInputMappingContextWithPriority
 * @brief Priority helps to determine the order of applying multiple input mapping contexts. But in most cases, it is enough to set the same priority for all contexts.
 */
USTRUCT(BlueprintType)
struct FInputMappingContextWithPriority
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UInputMappingContext> MappingContext = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Priority = 0;
};



/**
 * @class ACustomPlayerController
 * @brief It is a base class for player controllers in this project.
 */
UCLASS()
class PROJECT_A_API ACustomPlayerController : public APlayerController
{
	GENERATED_BODY()
	
							/**  === C++ member functions === */
public:
	ACustomPlayerController() = default;
	virtual ~ACustomPlayerController() = default;
		
	void OnFocusChanged(AActor* NewFocusedActor);
	void SetDeathMenuVisible(bool bVisible);

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnRep_Pawn() override;

	/** Input mapping context setup */
	/**
	 * @brief Client only. Iterates through `InputMappingContext` and adds non-empty `MappingContext` to UEnhancedInputLocalPlayerSubsystem`.
	 * @see  TArray<FInputMappingContextWithPriority> InputMappingContext
	 */
	virtual void SetupInputComponent() override;	
	/**
	 * @brief This function is called on the client after the server has confirmed possession of a pawn. It is responsible for setting up the player's HUD and other client-side elements based on the newly possessed pawn.
	 */
	virtual void AcknowledgePossession(APawn* InPawn) override;	

private:
	void ShowDeathMenu(); // helper
	void HideDeathMenu(); // helper

	/**
	 * @brief It checks if the pawn is valid and if the HUD can be initialized for it, and if so, it sets up the necessary widgets and UI elements.
	 */
	void TryInitialiseHudForPawn();

	/**
	 * @brief This flag is used to prevent multiple respawn requests from being sent to the server while waiting for a response. It is set to true when a respawn request is made and reset to false when the player successfully respawns or exits the death menu.
	 */
	bool bRespawnRequested{false};

	/**
	 * @brief Server function. Helper.
	 * 
	 * It sends the list of available world servers to the client, which can then display it in the HUD for the player to choose from.
	 */
	void PushWorldServerListToHud(const TArray<FWorldServerView>& Servers);

	/**
	 * @brief Server function. Helper.
	 *
	 * It sends the list of characters associated with the player's account.
	 */
	void PushCharacterListToHud(const TArray<FCharacterSelectionView>& Characters);


////// A simple way to store the selected character's information.
// see AWorldGameMode::CharacterDefinitionById
public:
	void SetSelectedCharacterId(FName CharacterId) { SelectedCharacterId = CharacterId; }
	FName GetSelectedCharacterId() const { return SelectedCharacterId; }
	void SetSelectedCharacterLevel(int32 Level) { SelectedCharacterLevel = Level; }
	int32 GetSelectedCharacterLevel() const { return SelectedCharacterLevel; }
private:
	FName SelectedCharacterId = NAME_None;
	int32 SelectedCharacterLevel = 1;
	UPROPERTY(Transient)
	TArray<TObjectPtr<UCharacterEntryObject>> CachedCharacterEntries{};
//////


						/* === Unreal Engine UFUNCTIONs === */
public:

	UFUNCTION(Client, Reliable)
	void Client_OnConnected(EServerWorldType World);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_RequestRespawn();

	/**
	 * @brief It just spawns an enemy in front of the player. Used for testing purposes only.
	 * 
	 * @param EnemyDataTable sets in Blueprint
	 * @param EnemyTag sets in Blueprint
	 */
	UFUNCTION(Server, Reliable, BlueprintCallable, meta = (DevelopmentOnly))
	void Server_SpawnEnemyInFront(UDataTable* EnemyDataTable, FGameplayTag EnemyTag);

	/**
	 * @brief It is called by client to request the list of available world servers from the hub-server. 
	 * @see AHubGameMode
	 * 
	 * @todo It should be implemented in another PlayerController class, which is used only in the hub-server. But for simplicity, it is implemented here for now.
	 */
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_RequestWorldServerList();

	/**
	 * @brief It is called by the hub-server to send the list of available world servers to the client. The client can then display this list in the HUD for the player to choose from.
	 * @see AHubGameMode
	 * 
	 * @todo It should be implemented in another PlayerController class, which is used only in the hub-server. But for simplicity, it is implemented here for now.
	 */
	UFUNCTION(Client, Reliable)
	void Client_ReceiveWorldServerList(const TArray<FWorldServerView>& Servers);

	/**
	 * @brief It is called by client to request the list of characters associated with the player's account on the hub-server. 
	 * In the current implementation, there is no actual account system, so the server just sends a mock list of characters for testing purposes.
	 * @see AHubGameMode
	 * 
	 * @todo It should be implemented in another PlayerController class, which is used only in the hub-server. But for simplicity, it is implemented here for now.
	 */
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_RequestCharacterList(FName ServerId);

	/**
	 * @brief It is called by the hub-server to send the list of characters associated with the player's account to the client. The client can then display this list in the HUD for the player to choose which character to use when entering the game world.
	 * @see AHubGameMode
	 * 
	 * @todo It should be implemented in another PlayerController class, which is used only in the hub-server. But for simplicity, it is implemented here for now.
	 */
	UFUNCTION(Client, Reliable)
	void Client_ReceiveCharacterList(const TArray<FCharacterSelectionView>& Characters);

	/**
	 * @brief It is called via Blueprint when the player selects a character and confirms their choice.
	 * @see AHubGameMode
	 * @todo It should be implemented in another PlayerController class
	 */
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_RequestTravelToWorldWithCharacter(FName ServerId, FName CharacterId);

	/**
	 * @brief Party
	 * 
	 * @param[in,out] PartyPawns
	 */
	UFUNCTION(Client, Reliable)
	void Client_UpdatePartyMembers(const TArray<APawn*>& PartyPawns);

protected:
	/**
	 * @brief Draws debug visualization for enemy spawn location and rotation on the client. Used for testing purposes only.
	 * 
	 * @param Location The location where the enemy is spawned.
	 * @param Rotation The rotation the enemy is facing.
	 */
	UFUNCTION(Client, Reliable, meta = (DevelopmentOnly))
	void Client_DrawEnemySpawnDebug(FVector Location, FRotator Rotation);

	UFUNCTION(Client, Reliable)
	void Client_OnFocusChanged(AActor* NewFocusedActor);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic)
	void ShowPopup(AActor* Actor);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic)
	void HidePopup();

	/*UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic)
	void ShowInteractionMenu(AActor* TargetActor, const TArray<FInteractionActionType>& Actions);*/

	/*UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic)
	void HideInteractionMenu();*/

	/*UFUNCTION(Client, Reliable)
	void Client_ShowInteractionMenu(AActor* TargetActor, const TArray<FInteractionActionType>& Actions);*/

private:
	UFUNCTION()
	void HandleDeathMenuRespawnRequested();

	UFUNCTION()
	void HandleDeathMenuExitRequested();

						
							/* === Unreal Engine UPROPERTY === */
protected:
	/**
	 * @brief Which keys or axes trigger which actions.
	 * @note This field must be set before running the game.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TArray<FInputMappingContextWithPriority> InputMappingContext{};

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UDeathMenuWidget> DeathMenuWidgetClass{};

	UPROPERTY(Transient)
	TObjectPtr<UDeathMenuWidget> DeathMenuWidget{};


//public:
//	//void ShowInteractionMenuLocal(AActor* TargetActor, const TArray<FInteractionActionType>& Actions);
//
//	UFUNCTION(BlueprintCallable)
//	void SubmitInteractionMenuAction(FGameplayTag ActionId);

private:
	UPROPERTY(Transient)
	TWeakObjectPtr<AActor> InteractionMenuTarget{};


								/** === Additional === */

#if WITH_DEV_AUTOMATION_TESTS
public:
	const TArray<FInputMappingContextWithPriority>& AutoTestGetInputMappingContext() const;
#endif // WITH_DEV_AUTOMATION_TESTS
	

};


//virtual void OnUnPossess() override;

///**
//	 * @brief Server function.
//	 *
//	 */
//void RespawnPlayer();

///**
// * @see AHubGameMode
// */
//UFUNCTION(Server, Reliable, BlueprintCallable)
//void Server_RequestTravelToWorldServer(FName ServerId);

///**
// * @brief Client requests to enter the game world on the Hub-server.
// * @note This function is called from Blueprint. Next step -> connect to the GameWorld server.
// * @see AHubGameMode::EnterToWorld
// */
//UFUNCTION(Server, Reliable, BlueprintCallable)
//void Server_RequestEnterToWorld();

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GeneralGameMode.h"
#include "GameplayTagContainer.h"

#include "CustomPlayerController.generated.h"

class UInputMappingContext;
class UDeathMenuWidget;

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

	/** Input mapping context setup */
	/**
	 * @brief Client only. Iterates through `InputMappingContext` and adds non-empty `MappingContext` to UEnhancedInputLocalPlayerSubsystem`.
	 * @see  TArray<FInputMappingContextWithPriority> InputMappingContext
	 */
	virtual void SetupInputComponent() override;

	virtual void OnPossess(APawn* InPawn) override;

protected:
	/**
	 * @brief This function is called on the client after the server has confirmed possession of a pawn. It is responsible for setting up the player's HUD and other client-side elements based on the newly possessed pawn.
	 */
	virtual void AcknowledgePossession(APawn* InPawn) override;

	virtual void OnRep_Pawn() override;

	void ShowDeathMenu();
	void HideDeathMenu();

private:
	/**
	 * @brief It checks if the pawn is valid and if the HUD can be initialized for it, and if so, it sets up the necessary widgets and UI elements.
	 */
	void TryInitialiseHudForPawn();

	/**
	 * @brief This flag is used to prevent multiple respawn requests from being sent to the server while waiting for a response. It is set to true when a respawn request is made and reset to false when the player successfully respawns or exits the death menu.
	 */
	bool bRespawnRequested{false};


						/* === Unreal Engine UFUNCTIONs === */
public:
	/**
	 * @brief Client requests to enter the game world on the Hub-server.
	 * @note This function is called from Blueprint. Next step -> connect to the GameWorld server.
	 * @see AHubGameMode::EnterToWorld
	 */
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_RequestEnterToWorld();

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

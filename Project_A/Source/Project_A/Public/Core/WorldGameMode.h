/*****************************************************************//**
 * \file   WorldGameMode.h
 * \brief  Default game mode for gameplay maps.
 * 
 * \date   May 2026
 *********************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "GeneralGameMode.h"
#include "Engine/StreamableManager.h"
#include "Character/CharacterSaveData.h"

#include "WorldGameMode.generated.h"

class UQuestDefinition;

/**
 * @struct FCharacterSpawnDefinition
 * @brief Temporary solution. Mocked definition of character spawn data. It is used in WorldGameMode to determine which Pawn class to spawn for a player based on the character they selected in the character selection screen.
 */
USTRUCT(BlueprintType)
struct FCharacterSpawnDefinition
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<APawn> PawnClass{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin="1", ClampMax="100"))
	int32 Level{};
};

/**
 * @struct FPlayerSessionData
 * @brief Per-player session data stored server-side on the World server.
 */
USTRUCT()
struct FPlayerSessionData
{
	GENERATED_BODY()

	FString AccountId{};
	FString SessionToken{};
	FName   CharacterId{NAME_None};
};

/**
 * @class AWorldGameMode
 * @brief Game mode for the main world map. 
 */
UCLASS(Config = NetSetCustom)
class PROJECT_A_API AWorldGameMode : public AGeneralGameMode
{
	GENERATED_BODY()
						/* === C++ member functions === */
public:
	AWorldGameMode();
	virtual ~AWorldGameMode() = default;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual EServerWorldType GetMapIdentifier() const override;	// custom AGeneralGameMode

	/**
	 * @brief It is a simple way to spawn different characters based on a player's choice on the character selection screen. 
	 * @todo In the future, it can be replaced by a more complex system that takes into account more factors (e.g., player's progress, inventory, etc.) to determine which character to spawn.
	 */
	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override; // Unreal AGameModeBase

	/**
	 * @brief Party
	 * 
	 * @param[in,out] RequesterPawn
	 * @param[in,out] TargetPawn
	 */
	UFUNCTION(BlueprintCallable)
	void TryJoinParty(APawn* RequesterPawn, APawn* TargetPawn);

	/**
	 * @brief Called by WorldGameMode after character save data is loaded.
	 * Applies saved position, health and quest progress to the newly spawned pawn.
	 */
	void ApplySaveDataToPawn(APawn* InPawn, const FCharacterSaveData& SaveData);

	/**
	 * @brief Collects current character state from the pawn and saves it via CharacterService.
	 * Called in Logout() before the player disconnects.
	 */
	void SaveCharacterFromPawn(APlayerController* PC);

	/** @brief Returns the session data for a connected PlayerController, or nullptr if not found. */
	const FPlayerSessionData* GetSessionData(APlayerController* PC) const;

protected:
	/**
	 * @brief It is overridden to get a character information provided by a player from the character selection screen on the Hub-server.
	 */
	virtual FString InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId,
		const FString& Options, const FString& Portal) override; // Unreal AGameModeBase

	virtual void PostLogin(APlayerController* NewPlayer) override;

	virtual void Logout(AController* Exiting) override;
	
private:
	// Heartbeat related functions
	void SendHeartbeat();
	void InitialiseHeartbeat();
	void CleanupHeartbeat();

	/**
	 * @brief Helper function to build a heartbeat payload string in the format "k=v;k2=v2;...". It is used in SendHeartbeat() to send server information to the Hub server.
	 * @see SendHeartbeat()
	 */
	FString BuildHeartbeatPayload() const;

	/**
	 * @brief PlayerCount is used in a WorldListScreen.
	 */
	int32 GetPlayerCount() const;
	
	/**
	 * @brief Party
	 * 
	 * @param[in,out] PartyId
	 */
	void PushPartyMembersToClients(FName PartyId);

	/**
	 * @brief Loads character save data and applies it to the player's pawn.
	 * Called from PostLogin after the pawn is ready.
	 */
	void LoadAndApplyCharacterData(APlayerController* PC);


	// Helpers LoadAndApplyCharacterData
private:
	/**
	 * @brief Try to obtain session data for a player. Returns true when valid session data is available.
	 */
	bool TryGetSessionForPlayer(APlayerController* PC, FString& OutAccountId, FString& OutSessionToken, FName& OutCharacterId) const;

	/**
	 * @brief Called when character save data successfully loaded for a player.
	 */
	void HandleCharacterLoaded(APlayerController* PC, const FString& AccountId, FName CharacterId, const FCharacterSaveData& SaveData);

	/**
	 * @brief Called when character load failed for a player.
	 */
	void HandleCharacterLoadError(const FString& AccountId, FName CharacterId, const FString& Error);


						/* === C++ member variables === */
private:
	/**
	 * @brief It is required to send heartbeats to the Hub server to keep the server list updated.
	 * @see AHubGameMode
	 */
	FSocket* HeartbeatSendSocket = nullptr;
	FTimerHandle HeartbeatSendTimerHandle{};

	int32 NextPartyNumericId{1};

	/**
	 * @brief Per-player session data: AccountId, SessionToken, CharacterId.
	 * Populated in InitNewPlayer, cleared in Logout.
	 */
	TMap<TWeakObjectPtr<APlayerController>, FPlayerSessionData> PlayerSessionDataMap{};

	
						/* === Unreal Engine UPROPERTY === */
private:
	/**
	 * @brief Unique identifier for the server. 
	 * In the current implementation, it is overridden by command line arguments.
	 */
	UPROPERTY(EditDefaultsOnly)
	FName ServerId;

	/**
	 * @brief Public name for a server list screen.
	 * In the current implementation, it is overridden by command line arguments.
	 */
	UPROPERTY(EditDefaultsOnly)
	FString ServerName;

	/**
	 * @brief Public address for a server list screen.
	 * In the current implementation, it is overridden by command line arguments.
	 */
	UPROPERTY(EditDefaultsOnly)
	FString PublicAddress;

	/**
	 * @brief Maximum number of players for a server list screen.
	 * In the current implementation, it is overridden by command line arguments.
	 */
	UPROPERTY(EditDefaultsOnly)
	int32 MaxPlayers;

private:
	/**
	 * @brief Temporary solution. A simple way to spawn different characters.
	 * @todo In the future, it can be replaced by a more complex system that takes into account more factors (e.g., player's progress, inventory, etc.) to determine which character to spawn.
	 * @see GetDefaultPawnClassForController_Implementation()
	 */
	UPROPERTY(EditDefaultsOnly)
	TMap<FName, FCharacterSpawnDefinition> CharacterDefinitionById{};

	UPROPERTY(EditDefaultsOnly, Config, Category = "Network")
	FString HubHeartbeatAddress{};

	UPROPERTY(EditDefaultsOnly, Config, Category = "Network")
	int32 HubHeartbeatPort{};
};

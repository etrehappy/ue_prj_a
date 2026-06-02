/*****************************************************************//**
 * \file   HubGameMode.h
 * \brief  Default game mode for the Hub server.
 * 
 * \date   May 2026
 *********************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "GeneralGameMode.h"
#include "WorldServerView.h"
#include "CharacterSelectionView.h"

#include "HubGameMode.generated.h"

/**
 * @class AHubGameMode
 * @brief It is used in the Hub map. A Hub-server is responsible for:
	- Tracking available world servers via heartbeats.
	- Providing server list to clients for world selection.
	- Handling player requests to enter world servers.
 */
UCLASS(Config = NetSetCustom)
class PROJECT_A_API AHubGameMode : public AGeneralGameMode
{
	GENERATED_BODY()
	
						/* === C++ member functions === */
public:
	AHubGameMode();
	virtual ~AHubGameMode() = default;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual EServerWorldType GetMapIdentifier() const override;
	virtual void Logout(AController* Exiting) override;
	/**
	 * @brief Server function.
	 * 
	 * A simple way to enter the world with different characters.
	 * @see ACustomPlayerController::PushCharacterListToHud
	 * @todo Check how to validate the character. In the current implementation there is no authentication or save system.
	 */
	void EnterToWorldWithCharacter(APlayerController* PC, FName ServerId, FName CharacterId);

	/**
	 * @brief Server function.
	 *
	 * Provides a list of available world servers to the client. 
	 */

	TArray<FWorldServerView> BuildWorldServersSnapshot() const;

	/**
	 * @brief Server function.
	 *
	 * Fetches the character list for the player from the CharacterService using their AccountId.
	 * @see UCharacterService
	 * @see ACustomPlayerController::Server_RequestCharacterList_Implementation
	 */
	void FetchAndSendCharacterListToPlayer(APlayerController* PC);	

protected:
	/**
	 * @brief Extracts AccountId and SessionToken from the Options URL string passed by the client on connect.
	 * Stores them in PlayerAccountIds / PlayerSessionTokens for later use.
	 */
	virtual FString InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal) override;

private:

///// Heartbeat block start

	/**
	 * @see AHubGameMode::RuntimeServers 	 
	 */
	void InitHeartbeatListener();

	/**
	 * @see AHubGameMode::RuntimeServers
	 */
	void ShutdownHeartbeatListener();

	/**
	 * @brief Non-blocking loop that reads all pending UDP datagrams.
	 * @see AHubGameMode::RuntimeServers
	 */
	void PollHeartbeats();

	/**
	 * @brief Responsibilities:
		- Walks RuntimeServers and marks servers as offline if not seen recently.
		- Uses HeartbeatTimeoutSeconds to determine staleness.
		- Keeps the runtime registry current for client queries.	
	 * 
	 * @see AHubGameMode::RuntimeServers
	 */
	void MarkTimedOutServers();

	/**
	 * @brief Read one pending UDP datagram into OutPayload. Returns true when a datagram was read.
	 */	 
	bool ReceiveNextHeartbeat(uint32 PendingSize, FString& OutPayload);

	 /**
	 * @brief Parse payload of format "k=v;k2=v2;..." into a key/value map.
	 */
	TMap<FString, FString> ParsePayloadToKV(const FString& Payload) const;

	/**
	 * @brief Validate KV and upsert runtime server entry. Returns true on success.
	 */
	bool TryProcessHeartbeatKV(const TMap<FString, FString>& KV);

	
	//// Helpers for FetchAndSendCharacterListToPlayer
	/**
	 * @brief Try to obtain account/session for a connected player. Returns true when AccountId is present.
	 */
	bool TryGetPlayerAuthData(APlayerController* PC, FString& OutAccountId, FString& OutSessionToken) const;

	/**
	 * @brief Called when character list is received for a player; forwards list to client UI.
	 */
	void HandleCharacterListSuccess(APlayerController* PC, const FString& AccountId, const TArray<FCharacterSelectionView>& Characters);

	/**
	 * @brief Called when character list fetch fails for a player; logs and forwards empty list to client.
	 */
	void HandleCharacterListError(APlayerController* PC, const FString& Error);
							
private:
	// Heartbeat used to track runtime servers.
	/**
	 * @brief A map of online world servers.
	 * AHubGameMode uses a simple heartbeat mechanism to track available world servers. Each server sends a UDP datagram with its info every few seconds. The HubGameMode listens for these heartbeats and updates this map accordingly. Clients query the HubGameMode for the list of available servers when they want to enter the world.
	 * @see AWorldGameMode
	 */
	TMap<FName, FWorldServerView> RuntimeServers{};

	TMap<FName, double> LastSeenByServerId{};
	FSocket* HeartbeatSocket = nullptr; // It is UDP-socket
	FTimerHandle HeartbeatPollTimerHandle{};
	FTimerHandle HeartbeatTimeoutTimerHandle{};
	float HeartbeatTimeoutSeconds = 8.0f;

protected:
	UPROPERTY(EditDefaultsOnly, Config, Category = "Network")
	int32 HubHeartbeatPort{};

///// Heartbeat block end

private:
	// Per-player auth data (server-side only, not replicated)
	/**
	 * @brief Maps each connected PlayerController to their AccountId.
	 * Populated in InitNewPlayer, cleared on disconnect.
	 */
	TMap<TWeakObjectPtr<APlayerController>, FString> PlayerAccountIds{};

	/**
	 * @brief Maps each connected PlayerController to their SessionToken.
	 */
	TMap<TWeakObjectPtr<APlayerController>, FString> PlayerSessionTokens{};


};

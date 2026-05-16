/*****************************************************************//**
 * \file   LobbyGameMode.h
 * \brief  
 * 
 * \date   May 2026
 *********************************************************************/
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "LobbyPlayerView.h"

#include "LobbyGameMode.generated.h"

class ALobbyPlayerController;
class ALobbyGameState;
/**
 * @struct FLobbyRoomRuntime
 * @brief Represents the runtime state of a lobby room.
 */
USTRUCT()
struct FLobbyRoomRuntime
{
	GENERATED_BODY()

	FName LobbyCode{NAME_None}; // Unique code for the lobby room

	TArray<TWeakObjectPtr<ALobbyPlayerController>> Players{}; // List of players currently in the lobby room

	bool bMatchStarting{false};
	int32 CurrentCountdownSeconds{-1}; // Remaining seconds in the countdown before the match starts, -1 if no countdown is active
	FTimerHandle CountdownTimerHandle{}; // used to manage the countdown process
};

/**
 * @class ALobbyGameMode 
 * @brief Default GameMode for the lobby level.
 * 
 * @details Responsible for managing lobby rooms, player states, and transitioning to the main game when a match starts.
 */
UCLASS(Config = NetSetCustom)
class PROJECT_A_API ALobbyGameMode : public AGameModeBase
{
	GENERATED_BODY()

						/* === C++ member functions === */
public:
	ALobbyGameMode();
	virtual ~ALobbyGameMode() = default;

	/**
	 * @brief It is a start point for a player in the lobby level. It is called when a new player successfully logs in.
	 */
	virtual void PostLogin(APlayerController* NewPlayer) override;

	/**
	 * @brief It is an end point for a player in the lobby level. It is called when a player successfully logs out.	 
	 * @param[in,out] Exiting The controller of the player who is exiting.
	 */
	virtual void Logout(AController* Exiting) override;

	/**
	 * @brief 
	 * 
	 * @param[in,out] Requester
	 * @see ALobbyPlayerController::Server_CreateLobby
	 */
	void CreateRoomForPlayer(ALobbyPlayerController* Requester);

	/**
	 * @brief 
	 * 
	 * @param[in,out] Requester
	 * @param[in,out] LobbyCode
	 * @see ALobbyPlayerController::Server_JoinLobby
	 */
	void JoinRoomForPlayer(ALobbyPlayerController* Requester, FName RoomCode);

	/**
	 * @brief 
	 * 
	 * @param[in,out] Requester
	 * @see ALobbyPlayerController::Server_LeaveLobby
	 */
	void LeaveRoomForPlayer(ALobbyPlayerController* Requester);

	/**
	 * @brief 
	 * 
	 * @param[in,out] Requester
	 * @param[in,out] bNewReady
	 * @see ALobbyPlayerController::Server_SetReadyStatus
	 */
	void SetReadyForPlayer(ALobbyPlayerController* Requester, bool bNewReady);

	/**
	 * @brief 
	 * 
	 * @param[in,out] Requester
	 * @param[in,out] CharacterId
	 * @see ALobbyPlayerController::Server_SetSelectedCharacter
	 */
	void SetSelectedCharacterForPlayer(ALobbyPlayerController* Requester, FName CharacterId);

	/**
	 * @brief ALobbyPlayerController::Server_ForceStartMatch
	 * 
	 * @param[in,out] Requester
	 */
	void RequestForceStart(ALobbyPlayerController* Requester);
	

private:
	
	/**
	 * @brief 
	 * 
	 * @param[in,out] RoomCode
	 * @see RequestForceStart, HandleRoomCountdownTick
	 */
	void StartMatchTravelForRoom(FName RoomCode);
	
	

	void HandleLobbyStateChanged();

	/**
	 * @brief Refreshes the overall lobby state, typically called after any significant change in the lobby.
	 * 
	 */
	void RefreshLobbyState();

	/**
	 * @brief Pushes the current state of the specified lobby room to all players in that room.
	 * 
	 * @param[in] RoomCode The code of the lobby room to push the state for.
	 */
	void PushRoomStateToPlayers(FName RoomCode);

	/**
	 * @brief Starts the countdown for the specified lobby room.
	 *
	 * @param[in] RoomCode The code of the lobby room to start the countdown for.
	 */
	void StartCountdownForRoom(FName RoomCode);

	/**
	 * @brief Cancels the countdown for the specified lobby room.
	 *
	 * @param[in] RoomCode The code of the lobby room to cancel the countdown for.
	 */
	void CancelCountdownForRoom(FName RoomCode);

	/**
	 * @brief Handles the countdown tick for the specified lobby room.
	 *
	 * @param[in] RoomCode The code of the lobby room to handle the countdown tick for.
	 */
	void HandleRoomCountdownTick(FName RoomCode);


	/**
	 * @brief Helper function. 
	 * 
	 * Generates a unique lobby code.
	 */
	FName GenerateLobbyCode() const;

	/**
	 * @brief Helper function. 
	 * 
	 * Checks whether the lobby can start based on the ready status of the players.
	 * 
	 * @param[in] RoomCode The code of the lobby room to check.
	 * @return true if the lobby can start, false otherwise
	 */
	bool CanStartByReady(FName RoomCode) const;

	/**
	 * @brief Helper function.
	 * 
	 * Tries to find the lobby room code for the given player controller. It looks up the RoomCodeByPlayer map to find the corresponding lobby code. 
	 * 
	 * @param[in] PlayerController The player controller to find the lobby room code for.
	 * @param[out] OutRoomCode The output parameter to store the found lobby room code. If the player is not in any lobby, it sets OutRoomCode to NAME_None.
	 * @return true if the lobby room code was found, false otherwise.
	 */
	bool TryGetPlayerRoomCode(ALobbyPlayerController* PlayerController, FName& OutRoomCode) const;

	/**
	 * @brief Helper function.
	 *
	 * Checks whether the given character ID is in the list of available character IDs for the lobby.
	 *
	 * @see AvailableCharacterIds
	 * @param[in] CharacterId The character ID to check.
	 * @return true if the character ID is allowed, false otherwise
	 */
	bool IsCharacterAllowed(FName CharacterId) const;
	
	/**
	 * @brief When a player presses the "Force Start" button, this function is called to assign random characters to all players in the lobby room. This is a fallback mechanism in case some players haven't selected their characters before the match starts.
	 * 
	 * @param[in] RoomCode
	 * @see RequestForceStart
	 */
	void AssignRandomCharactersForRoom(FName RoomCode);

	/**
	 * @brief Helper function.
	 * 
	 * @return true if the lobby room was found and the configuration was successfully applied, false otherwise.
	 * 
	 */
	bool SetRoomConfig(ALobbyPlayerController* Requester, FName RoomCode);


	// Helpers for cleaner LeaveRoomForPlayer implementation
	bool TryFetchPlayerRoom(ALobbyPlayerController * Requester, FName & OutOldRoomCode, FLobbyRoomRuntime * &OutRoom) const;
	void SendClientClearedRoomState(ALobbyPlayerController * Requester) const;
	void RemovePlayerReferencesFromRoom(FLobbyRoomRuntime * Room, ALobbyPlayerController * Requester);
	void ResetPlayerLobbyState(ALobbyPlayerController * Requester) const;
	void MaybeDestroyRoomIfEmpty(FName RoomCode, FLobbyRoomRuntime * Room);

	// Helpers for RefreshLobbyState implementation
	void UpdateMatchStartingState(const FLobbyRoomRuntime& Room, bool& bAnyMatchStarting, int32& MinCountdownSeconds) const;
	void AppendRoomPlayersToLobbyView(const FLobbyRoomRuntime& Room, int32& ConnectedPlayers, int32& ReadyPlayers, TArray<FLobbyPlayerView>& LobbyPlayersView, int32& ValidPlayersInRoom) const;
	void AppendLobbyRoomView(FName RoomCode, const FLobbyRoomRuntime& Room, int32 ValidPlayersInRoom, TArray<FLobbyRoomView>& LobbyRoomsView) const;

	// Helpers for PushRoomStateToPlayers implementation
	void GatherLobbyPlayersForRoom(const FLobbyRoomRuntime& Room, TArray<FLobbyPlayerView>& OutLobbyPlayers) const;
	void BroadcastRoomStateToPlayers(FName RoomCode, const FLobbyRoomRuntime& Room, const TArray<FLobbyPlayerView>& LobbyPlayers) const;

						/* === C++ member variables === */
private:
	/**
	 * @brief 
	 * FName — Unique code for the lobby room
	 * FLobbyRoomRuntime — Runtime state of the lobby room, including players, match starting status, countdown timer, etc.
	 * 
	 * @see FLobbyRoomRuntime
	 */
	TMap<FName, FLobbyRoomRuntime> RoomsByCode{};

	/**
	 * @brief Maps each player controller to the lobby code of the room they are currently in.
	 * FName — Unique code for the lobby room
	 * TWeakObjectPtr<ALobbyPlayerController> — Weak pointer to the player controller to avoid dangling references when players disconnect or leave the lobby.
	 */
	TMap<TWeakObjectPtr<ALobbyPlayerController>, FName> RoomCodeByPlayer{};

	
						/* === Unreal Engine UPROPERTY === */
private:
	/**
	 * @brief Minimum number of players required to start the match.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Lobby")
	int32 MinPlayersToStart{2};

	/**
	 * @brief Number of seconds for the countdown before the match starts after the conditions are met (e.g., all players are ready).
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Lobby")
	int32 CountdownSeconds{5};

	/**	 
	 * @todo It should be more complex and involve permissions, host vs. clients, etc.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Lobby")
	bool bAllowAnyPlayerForceStart{true};

	/**
	 * @brief A simple way to configure which characters are available in the lobby. 
	 * @todo In a real project, this would likely be more complex and data-driven.
	 */
	UPROPERTY(EditDefaultsOnly)
	TArray<FName> AvailableCharacterIds{};

	UPROPERTY(EditDefaultsOnly, Config, Category = "Network")
	FString LobbyTargetWorldAddress{};
};

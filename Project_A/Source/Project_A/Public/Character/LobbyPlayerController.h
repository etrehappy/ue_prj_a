/*****************************************************************//**
 * \file   LobbyPlayerController.h
 * \brief  
 * 
 * \date   May 2026
 *********************************************************************/
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "LobbyPlayerView.h"

#include "LobbyPlayerController.generated.h"

class ALobbyGameState;
class ALobbyPlayerState;
class ALobbyHud;

/**
 * @class ALobbyPlayerController 
 * @brief Player controller for handling lobby interactions and state updates.
 */
UCLASS()
class PROJECT_A_API ALobbyPlayerController : public APlayerController
{
	GENERATED_BODY()


						/* === C++ member functions === */
public:
	ALobbyPlayerController() = default;
	virtual ~ALobbyPlayerController() = default;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void OnRep_PlayerState() override;

private:	
	void PushLobbyStateToHud();
	void HandleLobbyStateChanged();

	void CleanupCachedRoomState();


private:
	// Helpers extracted from PushLobbyStateToHud
	FName ResolveLobbyCodeToUse(const ALobbyPlayerState* LobbyPlayerState) const;
	int32 CountRoomReadyPlayers() const;
	void PushRoomCacheToHud(ALobbyHud* LobbyHud, FName LobbyCodeToUse) const;
	void PushGameStateToHud(ALobbyHud* LobbyHud, FName LobbyCodeToUse, const ALobbyGameState* LobbyGameState, const ALobbyPlayerState* LobbyPlayerState) const;

	/**
	 * @brief Helper function.
	 * 
	 * Binds to the lobby game state to receive updates about lobby state changes, and pushes the initial state to the HUD. Should be called in BeginPlay.
	 *
	 */
	void BindToLobbyGameState();

	/**
	 * @brief Helper function.
	 * 
	 * Unbinds from the lobby game state to stop receiving updates about lobby state changes. Should be called in EndPlay.
	 *
	 */
	void UnbindFromLobbyGameState();


						/* === C++ member variables === */
private:
	// Cached data to minimize the amount of data we need to push from GameState to HUD on every update. 
	// Updated when Client_UpdateLobbyRoomState is called, and used in PushLobbyStateToHud.

	TWeakObjectPtr<ALobbyGameState> CachedLobbyGameState{};	
	FName CachedClientLobbyCode{NAME_None};
	double LastRoomRpcTime{0.0};
	bool bHasCachedRoomState{false};
	TArray<FLobbyPlayerView> CachedRoomLobbyPlayers{};
	bool bCachedRoomMatchStarting{false};
	int32 CachedRoomCountdownSeconds{-1};
	TArray<FName> CachedRoomAvailableCharacterIds{};
	FName CachedRoomSelectedCharacterId{NAME_None};
	bool bCachedRoomLocalReady{false};

	//


						/* === Unreal Engine UFUNCTION === */
public:	

	UFUNCTION(BlueprintCallable, Category = "Lobby")
	void SetSelectedCharacterId(FName CharacterId);	

	/**
	 * @brief It is called by the server to update the lobby room state for this player. 
	 */
	UFUNCTION(Client, Reliable)
	void Client_UpdateLobbyRoomState(FName LobbyCode, const TArray<FLobbyPlayerView>& LobbyPlayers,
		bool bMatchStarting, int32 CountdownSeconds, bool bLocalReady, const TArray<FName>& AvailableCharacterIds,
		FName SelectedCharacterId);

protected:

	UFUNCTION(BlueprintCallable, Category = "Lobby")
	void SetReadyStatus(bool bNewIsReady);

	UFUNCTION(BlueprintCallable, Category = "Lobby")
	void ForceStartMatch();		

	UFUNCTION(BlueprintCallable, Category = "Lobby")
	void CreateLobby();

	UFUNCTION(BlueprintCallable, Category = "Lobby")
	void JoinLobby(FName LobbyCode);

	UFUNCTION(BlueprintCallable, Category = "Lobby")
	void LeaveLobby();

	UFUNCTION(Server, Reliable)
	void Server_SetSelectedCharacterId(FName CharacterId);

	UFUNCTION(Server, Reliable)
	void Server_SetReadyStatus(bool bNewIsReady);

	UFUNCTION(Server, Reliable)
	void Server_ForceStartMatch();

	UFUNCTION(Server, Reliable)
	void Server_CreateLobby();

	UFUNCTION(Server, Reliable)
	void Server_JoinLobby(FName LobbyCode);

	UFUNCTION(Server, Reliable)
	void Server_LeaveLobby();
};

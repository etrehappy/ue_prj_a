/*****************************************************************//**
 * \file   LobbyGameState.h
 * \brief  
 * 
 * \date   May 2026
 *********************************************************************/
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "LobbyPlayerView.h"

#include "LobbyGameState.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnLobbyStateChangedNative);

/**
 * @class ALobbyGameState
 * @brief Represents the state of the lobby, replicated to all clients.
 */
UCLASS()
class PROJECT_A_API ALobbyGameState : public AGameStateBase
{
	GENERATED_BODY()

						/* === C++ member functions === */
public:
	ALobbyGameState() = default;
	virtual ~ALobbyGameState() = default;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void SetLobbySnapshot(int32 NewConnectedPlayers, int32 NewReadyPlayers, bool bNewMatchStarting,
		int32 NewCountdownSeconds, const TArray<FName>& NewAvailableCharacterIds,
		const TArray<FLobbyPlayerView>& NewLobbyPlayers, const TArray<FLobbyRoomView>& NewLobbyRooms);

	const TArray<FName>& GetAvailableCharacterIds() const { return AvailableCharacterIds; }	
	int32 GetConnectedPlayers() const { return ConnectedPlayers; }
	int32 GetReadyPlayers() const { return ReadyPlayers; }
	bool IsMatchStarting() const { return bMatchStarting; }
	int32 GetCountdownSeconds() const { return CountdownSeconds; }
	const TArray<FLobbyPlayerView>& GetLobbyPlayers() const { return LobbyPlayers; }
	const TArray<FLobbyRoomView>& GetLobbyRooms() const { return LobbyRooms; }	

private:
	void NotifyLobbyStateChanged();

						/* === C++ member variables === */
public:
	FOnLobbyStateChangedNative OnLobbyStateChangedNative{};

		
						/* === Unreal Engine UFUNCTION === */
protected:	
	UFUNCTION(BlueprintImplementableEvent, Category = "Lobby")
	void OnLobbyStateChanged();

	UFUNCTION()
	void OnRep_LobbyState();

						/* === Unreal Engine UPROPERTY === */
protected:
	UPROPERTY(ReplicatedUsing = OnRep_LobbyState, BlueprintReadOnly)
	int32 ConnectedPlayers{0};

	UPROPERTY(ReplicatedUsing = OnRep_LobbyState, BlueprintReadOnly)
	int32 ReadyPlayers{0};

	UPROPERTY(ReplicatedUsing = OnRep_LobbyState, BlueprintReadOnly)
	bool bMatchStarting{false};

	UPROPERTY(ReplicatedUsing = OnRep_LobbyState, BlueprintReadOnly)
	int32 CountdownSeconds{-1};

	UPROPERTY(ReplicatedUsing = OnRep_LobbyState, BlueprintReadOnly)
	TArray<FName> AvailableCharacterIds{};

	UPROPERTY(ReplicatedUsing = OnRep_LobbyState, BlueprintReadOnly)
	TArray<FLobbyPlayerView> LobbyPlayers{};

	UPROPERTY(ReplicatedUsing = OnRep_LobbyState, BlueprintReadOnly)
	TArray<FLobbyRoomView> LobbyRooms{};
};



#include "Core/LobbyGameState.h"

#include "Net/UnrealNetwork.h"

void ALobbyGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALobbyGameState, ConnectedPlayers);
	DOREPLIFETIME(ALobbyGameState, ReadyPlayers);
	DOREPLIFETIME(ALobbyGameState, bMatchStarting);
	DOREPLIFETIME(ALobbyGameState, CountdownSeconds);
	DOREPLIFETIME(ALobbyGameState, AvailableCharacterIds);
	DOREPLIFETIME(ALobbyGameState, LobbyPlayers);
	DOREPLIFETIME(ALobbyGameState, LobbyRooms);
}

void ALobbyGameState::SetLobbySnapshot(int32 NewConnectedPlayers, int32 NewReadyPlayers,
	bool bNewMatchStarting,	int32 NewCountdownSeconds, const TArray<FName>& NewAvailableCharacterIds,
	const TArray<FLobbyPlayerView>& NewLobbyPlayers, const TArray<FLobbyRoomView>& NewLobbyRooms)
{
	bool bChanged = false;

	if (ConnectedPlayers != NewConnectedPlayers)
	{
		ConnectedPlayers = NewConnectedPlayers;
		bChanged = true;
	}

	if (ReadyPlayers != NewReadyPlayers)
	{
		ReadyPlayers = NewReadyPlayers;
		bChanged = true;
	}

	if (bMatchStarting != bNewMatchStarting)
	{
		bMatchStarting = bNewMatchStarting;
		bChanged = true;
	}

	if (CountdownSeconds != NewCountdownSeconds)
	{
		CountdownSeconds = NewCountdownSeconds;
		bChanged = true;
	}

	if (AvailableCharacterIds != NewAvailableCharacterIds)
	{
		AvailableCharacterIds = NewAvailableCharacterIds;
		bChanged = true;
	}

	if (LobbyPlayers != NewLobbyPlayers)
	{
		LobbyPlayers = NewLobbyPlayers;
		bChanged = true;
	}

	if (LobbyRooms != NewLobbyRooms)
	{
		LobbyRooms = NewLobbyRooms;
		bChanged = true;
	}

	if (bChanged)
	{
		NotifyLobbyStateChanged();
	}
}

void ALobbyGameState::NotifyLobbyStateChanged()
{
	OnLobbyStateChangedNative.Broadcast();
	OnLobbyStateChanged();
}

void ALobbyGameState::OnRep_LobbyState()
{
	NotifyLobbyStateChanged();
}
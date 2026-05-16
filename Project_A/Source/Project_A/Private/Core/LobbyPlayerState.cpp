#include "Core/LobbyPlayerState.h"

#include "Net/UnrealNetwork.h"

void ALobbyPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALobbyPlayerState, bIsReady);
	DOREPLIFETIME(ALobbyPlayerState, SelectedCharacterId);
	DOREPLIFETIME(ALobbyPlayerState, LobbyCode);
}

bool ALobbyPlayerState::InisialiseLobbyPlayerState(bool bInIsReady, FName InSelectedCharacterId, FName InRoomCode)
{
	if (InRoomCode.IsNone())
	{
		return false;
	}

	SetLobbyCode(InRoomCode);
	SetReadyStatus(bInIsReady);
	SetSelectedCharacterId(InSelectedCharacterId);

	return true;
}

void ALobbyPlayerState::SetReadyStatus(bool bNewIsReady)
{
	if (bIsReady == bNewIsReady)
	{
		return;
	}

	bIsReady = bNewIsReady;
}


void ALobbyPlayerState::SetSelectedCharacterId(FName InSelectedCharacterId)
{
	if (SelectedCharacterId == InSelectedCharacterId)
	{
		return;
	}

	SelectedCharacterId = InSelectedCharacterId;
}


void ALobbyPlayerState::SetLobbyCode(FName InLobbyCode)
{
	if (LobbyCode == InLobbyCode)
	{
		return;
	}

	LobbyCode = InLobbyCode;
}


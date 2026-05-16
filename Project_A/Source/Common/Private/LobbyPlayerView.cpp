


#include "LobbyPlayerView.h"


bool FLobbyPlayerView::operator==(const FLobbyPlayerView& Other) const
{
	return PlayerNumber == Other.PlayerNumber
		&& PlayerId == Other.PlayerId
		&& bIsReady == Other.bIsReady;
}

bool FLobbyRoomView::operator==(const FLobbyRoomView& Other) const
{
	return LobbyCode == Other.LobbyCode
		&& PlayersCount == Other.PlayersCount
		&& bMatchStarting == Other.bMatchStarting;
}
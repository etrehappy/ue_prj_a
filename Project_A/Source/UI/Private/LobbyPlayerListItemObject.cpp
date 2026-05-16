


#include "LobbyPlayerListItemObject.h"

void ULobbyPlayerListItemObject::Initialise(const FLobbyPlayerView& InView)
{
	PlayerNumber = InView.PlayerNumber;
	PlayerId = InView.PlayerId;
	bIsReady = InView.bIsReady;
}
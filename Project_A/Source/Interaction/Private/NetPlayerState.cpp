


#include "NetPlayerState.h"

#include "NetPlayerState.h"

#include "Net/UnrealNetwork.h"

void ANetPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ANetPlayerState, PartyId);
}


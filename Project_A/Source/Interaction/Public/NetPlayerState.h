/*****************************************************************//**
 * \file   NetPlayerState.h
 * \brief  Custom PlayerState class for networked gameplay, including party management.
 * 
 * \date   May 2026
 *********************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "NetPlayerState.generated.h"

/**
 * @class ANetPlayerState
 * @brief
 * @see AWorldGameMode
 */
UCLASS()
class INTERACTION_API ANetPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void SetPartyId(FName InPartyId) { PartyId = InPartyId; }
	FName GetPartyId() const { return PartyId; }

private:
	UPROPERTY(Replicated)
	FName PartyId{NAME_None};
	
	
};

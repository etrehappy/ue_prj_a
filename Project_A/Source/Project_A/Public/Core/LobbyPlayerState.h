/*****************************************************************//**
 * \file   LobbyPlayerState.h
 * \brief  
 * 
 * \date   May 2026
 *********************************************************************/
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"

#include "LobbyPlayerState.generated.h"

/**
 * @class ALobbyPlayerState
 * @brief It represents the state of a player in the lobby. It is replicated to ensure that all clients have up-to-date information about each player's state in the lobby.
 */
UCLASS()
class PROJECT_A_API ALobbyPlayerState : public APlayerState
{
	GENERATED_BODY()

						/* === C++ member functions === */
public:
	ALobbyPlayerState() = default;
	virtual ~ALobbyPlayerState() = default;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	bool InisialiseLobbyPlayerState(bool bInIsReady, FName InSelectedCharacterId, FName InRoomCode);
	void SetReadyStatus(bool bNewIsReady);
	void SetSelectedCharacterId(FName InSelectedCharacterId);
	void SetLobbyCode(FName InLobbyCode);

	bool IsReadyStatus() const { return bIsReady; }
	FName GetSelectedCharacterId() const { return SelectedCharacterId; }
	FName GetLobbyCode() const { return LobbyCode; }

						/* === Unreal Engine UPROPERTY === */
protected:
	UPROPERTY(Replicated, BlueprintReadOnly)
	bool bIsReady{false};

	UPROPERTY(Replicated, BlueprintReadOnly)
	FName SelectedCharacterId{NAME_None};

	UPROPERTY(Replicated, BlueprintReadOnly)
	FName LobbyCode{NAME_None};
};

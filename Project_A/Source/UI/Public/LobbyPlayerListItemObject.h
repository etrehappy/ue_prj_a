#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "LobbyPlayerView.h"

#include "LobbyPlayerListItemObject.generated.h"

/**
 * @class ULobbyPlayerListItemObject
 * @brief Represents a lobby player list item, used for displaying player information in the UI.
 */
UCLASS(BlueprintType)
class UI_API ULobbyPlayerListItemObject : public UObject
{
	GENERATED_BODY()

public:
	void Initialise(const FLobbyPlayerView& InView);
	

	UPROPERTY(BlueprintReadOnly, Category = "Lobby")
	int32 PlayerNumber{0};

	UPROPERTY(BlueprintReadOnly, Category = "Lobby")
	int32 PlayerId{-1};

	UPROPERTY(BlueprintReadOnly, Category = "Lobby")
	bool bIsReady{false};
};
#pragma once

#include "CoreMinimal.h"

#include "LobbyPlayerView.generated.h"

/**
 * @struct FLobbyPlayerView
 * @brief Represents the view of a player in the lobby, used for displaying player information in the UI. 
 */
USTRUCT(BlueprintType)
struct COMMON_API FLobbyPlayerView
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 PlayerNumber{0};

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 PlayerId{-1};

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bIsReady{false};

	bool operator==(const FLobbyPlayerView& Other) const;
};

/**
 * @struct FLobbyRoomView
 * @brief Represents the view of a lobby room, used for displaying room information in the UI.
 */
USTRUCT(BlueprintType)
struct COMMON_API FLobbyRoomView
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName LobbyCode{NAME_None};

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 PlayersCount{0};

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bMatchStarting{false};

	bool operator==(const FLobbyRoomView& Other) const;
};

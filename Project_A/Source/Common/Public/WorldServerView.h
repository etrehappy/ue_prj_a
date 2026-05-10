/*****************************************************************//**
 * \file   WorldServerView.h
 * \brief  
 * 
 * \date   May 2026
 *********************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"

#include "WorldServerView.generated.h"

/**
 * @struct FWorldServerView
 * @brief Represents the view of a game world server. Used for displaying server information in the UI.
 */
USTRUCT(BlueprintType)
struct FWorldServerView
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName ServerId = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString ServerName{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString Address{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 CurrentPlayers = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 MaxPlayers = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bIsOnline = false;
};

/**
 * @class UWorldServerEntryObject
 * @brief UObject wrapper for FWorldServerView to be used in WBP.
 */
UCLASS(BlueprintType)
class COMMON_API UWorldServerEntryObject : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite)
	FWorldServerView Data;
};

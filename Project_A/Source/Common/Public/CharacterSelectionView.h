/*****************************************************************//**
 * \file   CharacterSelectionView.h
 * \brief  
 * 
 * \date   May 2026
 *********************************************************************/
#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"

#include "CharacterSelectionView.generated.h"

 /**
  * @struct FWorldServerView
  * @brief Represents the view of a character in the character selection screen.
  */
USTRUCT(BlueprintType)
struct FCharacterSelectionView
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName CharacterId = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString CharacterName{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Level = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName ClassName{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bCanEnter = true;
};

/**
 * @class UCharacterEntryObject
 * @brief UObject wrapper for FCharacterSelectionView to be used in WBP.
 */
UCLASS(BlueprintType)
class COMMON_API UCharacterEntryObject : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	FCharacterSelectionView Data;
};
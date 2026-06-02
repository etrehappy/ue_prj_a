
/*****************************************************************//**
 * \file   CharacterSaveData.h
 * \brief  Data structures for character persistence.
 *
 * \date   May 2026
 *********************************************************************/
#pragma once

#include "CoreMinimal.h"
#include "CharacterSaveData.generated.h"

 /**
  * @struct FInventoryItemSave
  * @brief Minimal inventory item snapshot for persistence.
  */
USTRUCT(BlueprintType)
struct FInventoryItemSave
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FName ItemId{NAME_None};

	UPROPERTY(BlueprintReadWrite)
	int32 Quantity{1};
};

/**
 * @struct FQuestSave
 * @brief Minimal quest progress snapshot for persistence.
 */
USTRUCT(BlueprintType)
struct FQuestSave
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FName QuestId{NAME_None};

	/** @brief 0 = in progress, 1 = completed, 2 = failed */
	UPROPERTY(BlueprintReadWrite)
	int32 Status{0};

	UPROPERTY(BlueprintReadWrite)
	int32 CurrentStep{0};
};

/**
 * @struct FCharacterSaveData
 * @brief Full character state snapshot used for saving and loading.
 */
USTRUCT(BlueprintType)
struct FCharacterSaveData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FName CharacterId{NAME_None};

	UPROPERTY(BlueprintReadWrite)
	FString CharacterName{};

	UPROPERTY(BlueprintReadWrite)
	int32 Level{1};

	UPROPERTY(BlueprintReadWrite)
	FString ClassName{};

	UPROPERTY(BlueprintReadWrite)
	int32 Experience{0};

	UPROPERTY(BlueprintReadWrite)
	float CurrentHealth{100.f};

	UPROPERTY(BlueprintReadWrite)
	float MaxHealth{100.f};

	UPROPERTY(BlueprintReadWrite)
	FVector LastPosition{FVector::ZeroVector};

	UPROPERTY(BlueprintReadWrite)
	FRotator LastRotation{FRotator::ZeroRotator};
		
	UPROPERTY(BlueprintReadWrite)
	FName LastMapId{NAME_None};

	UPROPERTY(BlueprintReadWrite)
	TArray<FInventoryItemSave> Inventory{};

	UPROPERTY(BlueprintReadWrite)
	TArray<FQuestSave> Quests{};

	bool IsValid() const { return CharacterId != NAME_None; }
};
/*****************************************************************//**
 * \file   QuestTypes.h
 * \brief  
 * 
 * \date   May 2026
 *********************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

#include "QuestTypes.generated.h"

/**
 * @enum EQuestSharingMode
 * @brief Defines how a quest is shared among players.
 */
UENUM(BlueprintType)
enum class EQuestSharingMode : uint8
{
	Individual UMETA(DisplayName = "Individual"),
	Party UMETA(DisplayName = "Party")
};

/**
 * @enum EQuestStatus
 * @brief Defines the current status of a quest.
 */
UENUM(BlueprintType)
enum class EQuestStatus : uint8
{
	None UMETA(DisplayName = "None"),
	Accepted UMETA(DisplayName = "Accepted"),
	Completed UMETA(DisplayName = "Completed"),
	TurnedIn UMETA(DisplayName = "TurnedIn")
};

/**
 * @struct FQuestObjectiveRuntime
 * @brief Represents the runtime state of a quest objective.
 */
USTRUCT(BlueprintType)
struct FQuestObjectiveRuntime
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag ObjectiveTag{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0"))
	int32 CurrentValue{0};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "1"))
	int32 TargetValue{1};
};

/**
 * @struct FQuestInstance
 * @brief Represents an instance of a quest.
 */
USTRUCT(BlueprintType)
struct FQuestInstance
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName QuestId{NAME_None};

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EQuestSharingMode SharingMode{EQuestSharingMode::Individual};

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EQuestStatus Status{EQuestStatus::None};

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FQuestObjectiveRuntime> Objectives{};
};
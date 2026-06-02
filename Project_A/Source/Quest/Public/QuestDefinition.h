/*****************************************************************//**
 * \file   QuestDefinition.cpp
 * \brief Contains:
 * - enum EQuestObjectiveKind - how the progress is tracked (for example, "go to location" or "collect 10 items")
 * - struct FQuestObjectiveDefinition - a single objective in a quest
 * - class UQuestDefinition - defines a quest, including its objectives, description, and other metadata
 * - class UQuestTrackRowObject - represents a row in the quest tracking UI
 *
 * \date   May 2026
 *********************************************************************/
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "QuestTypes.h"
#include "GameplayTagContainer.h"

#include "QuestDefinition.generated.h"

/**
 * @enum EQuestObjectiveKind
 * @brief Defines the type of the quest objective, which determines how the progress is tracked and updated at runtime.
 */
UENUM(BlueprintType)
enum class EQuestObjectiveKind : uint8
{
	EventDelta UMETA(DisplayName = "EventDelta"),
	InventoryItemCount UMETA(DisplayName = "InventoryItemCount")
};

USTRUCT(BlueprintType)
struct FQuestObjectiveDefinition
{
	GENERATED_BODY()

	/**
	 * @brief Unique tag to identify the objective. It is used to track progress and update the quest instance at runtime.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Quest")
	FGameplayTag ObjectiveTag{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Quest")
	EQuestObjectiveKind ObjectiveKind{EQuestObjectiveKind::EventDelta};

	/**
	 * @brief It is used to specify the type of item that the objective is related to. For example, if the objective is "Kill 5 goblins", the tag can be "Goblin".
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Quest", meta = (EditCondition = "ObjectiveKind == EQuestObjectiveKind::InventoryItemCount"))
	FGameplayTag RequiredItemTypeTag{};

	/**
	 * @brief How much progress is needed to complete the objective. For example, if the objective is "Kill 5 goblins", the tag can be "KillGoblin" and the target value can be 5. The actual progress will be tracked in the quest instance at runtime.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Quest", meta = (ClampMin = "1"))
	int32 TargetValue{1};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Quest")
	bool bShowValueInUI{false};

	/**
	 * @brief It is a description of the objective that can be displayed in the UI. It can contain dynamic information about the objective.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Quest")
	FText Description{};	
};

/**
 * @brief At runtime, a copy of this definition is used to create a quest instance for each player that accepts the quest.
 * Loaded via .ini
 */
UCLASS(BlueprintType)
class QUEST_API UQuestDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Quest")
	FName QuestId{NAME_None};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Quest")
	FText DisplayName{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Quest")
	TObjectPtr<UTexture2D> QuestImage{nullptr};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Quest")
	FName QuestGiver{NAME_None};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Quest")
	FName QuestAcceptor{NAME_None};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Quest", meta = (MultiLine = "true"))
	FText Description{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Quest")
	EQuestSharingMode SharingMode{EQuestSharingMode::Individual};

	/**
	 * @brief It is steps to complete the quest. 
	 * All objectives are considered active once the quest is accepted, but they can be completed in any order.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Quest")
	TArray<FQuestObjectiveDefinition> Objectives{};

	
	/**
	 * @brief Creates a runtime copy of the quest for QuestLogComponent
	 */
	bool BuildInitialQuestInstance(FQuestInstance& OutQuestInstance) const;
};


/**
 * @class UQuestTrackRowObject
 * @brief UI wrapper for FQuestObjectiveDefinition to be used in WBP.
 */
UCLASS(BlueprintType)
class QUEST_API UQuestTrackRowObject : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Quest")
	FText ObjectiveDescription{}; // in the row

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Quest", meta = (ClampMin = "1"))
	int32 TargetValue{1};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Quest", meta = (ClampMin = "1"))
	int32 CurrentValue{1};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Quest")
	bool bShowValueInUI{false};

	UFUNCTION(BlueprintCallable, Category = "Quest")
	static UQuestTrackRowObject* CreateQuestTrackRow(const FText& InDescription, int32 InTargetValue,
		int32 InCurrentValue, bool bInShowValueInUI);
};

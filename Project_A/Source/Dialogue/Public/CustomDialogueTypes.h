/*****************************************************************//**
 * \file   CustomDialogueTypes.h
 * \brief  Contains
 * - enum class EDialogueNodeKind — type of dialogue node (line, choice hub, end)
 * - enum class EDialogueQuestStateRequirement — quest state requirement for dialogue condition
 * - class UDialogueCondition — for example, to check quest state or inventory
 * - class UDialogueEffect — for example, to update quest state
 * 
 * \date   May 2026
 *********************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GameplayTagContainer.h"

#include "CustomDialogueTypes.generated.h"

struct FDialogueRuntimeContext;

/**
 * @enum EDialogueNodeKind
 * @brief Type of dialogue node.
 */
UENUM(BlueprintType)
enum class EDialogueNodeKind : uint8
{
	Line = 0,
	ChoiceHub,
	End
};

/**
 * @enum EDialogueQuestStateRequirement
 * @brief Quest state requirement for dialogue condition.
 */
UENUM(BlueprintType)
enum class EDialogueQuestStateRequirement : uint8
{
	NotAccepted = 0,
	Accepted,
	ReadyToTurnIn,
	TurnedIn
};



///////////////////////////////////////////////////////////////////////////////
/// 
/// Conditions
/// 
///////////////////////////////////////////////////////////////////////////////

/**
 * @class UDialogueCondition
 * @brief Determines whether a dialogue node or choice is available. For example, a condition can check if the player has a specific quest or item.
 * @see FDialogueChoiceDefinition
 * @see FDialogueNodeDefinition
 */
UCLASS(Abstract, BlueprintType, Blueprintable, EditInlineNew, DefaultToInstanced)
class DIALOGUE_API UDialogueCondition : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Condition")
	FName ConditionId{NAME_None};

	/**
	 * @brief Server's runtime check.
	 */
	virtual bool IsSatisfied(const FDialogueRuntimeContext& RuntimeContext) const;
};


/**
 * @class UDialogueConditionQuestState
 * @brief Checks player's quest state. 
 */
UCLASS(BlueprintType, EditInlineNew, DefaultToInstanced)
class DIALOGUE_API UDialogueConditionQuestState : public UDialogueCondition
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Quest")
	FName QuestId{NAME_None};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Quest")
	EDialogueQuestStateRequirement RequiredState{EDialogueQuestStateRequirement::NotAccepted};

	virtual bool IsSatisfied(const FDialogueRuntimeContext& RuntimeContext) const override;
};




///////////////////////////////////////////////////////////////////////////////
/// 
/// Effects
/// 
///////////////////////////////////////////////////////////////////////////////

/**
 * @class UDialogueEffect
 * @brief Represents an effect that can be applied during a dialogue. For example, accepting a quest, giving an item, or triggering an interaction.
 * @see FDialogueChoiceDefinition
 * @see FDialogueNodeDefinition
 */
UCLASS(Abstract, BlueprintType, Blueprintable, EditInlineNew, DefaultToInstanced)
class DIALOGUE_API UDialogueEffect : public UObject
{
	GENERATED_BODY()

public:
	/**
	 * @brief Optional identifier for debugging and analytics.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect")
	FName EffectId{NAME_None};

	/**
	 * @brief Server's runtime execution.
	 */
	virtual void Apply(const FDialogueRuntimeContext& RuntimeContext) const {};
};


/**
 * @class UDialogueEffectApplyInteractionAction
 * @brief Applies an interaction action to the NPC. For example, to open a shop UI.
 */
UCLASS(BlueprintType, EditInlineNew, DefaultToInstanced)
class DIALOGUE_API UDialogueEffectApplyInteractionAction : public UDialogueEffect
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction")
	FGameplayTag ActionTag{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction")
	bool bCheckCanInteract{true};

	virtual void Apply(const FDialogueRuntimeContext& RuntimeContext) const override;
};

/**
 * @class UDialogueEffectAcceptQuest
 * @brief Accepts a quest for the player.
 */
UCLASS(BlueprintType, EditInlineNew, DefaultToInstanced)
class DIALOGUE_API UDialogueEffectAcceptQuest : public UDialogueEffect
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Quest")
	FName QuestId{NAME_None};

	virtual void Apply(const FDialogueRuntimeContext& RuntimeContext) const override;
};

/**
 * @class UDialogueEffectTurnInQuest
 * @brief Turns in a quest for the player. 
 */
UCLASS(BlueprintType, EditInlineNew, DefaultToInstanced)
class DIALOGUE_API UDialogueEffectTurnInQuest : public UDialogueEffect
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Quest")
	FName QuestId{NAME_None};

	virtual void Apply(const FDialogueRuntimeContext& RuntimeContext) const override;
};

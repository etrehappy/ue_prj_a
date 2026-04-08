/*****************************************************************//**
 * \file   StatusEffectTypes.h
 * \brief  Defines types and structures related to status effects.
 * 
 * \date   April 2026
 *********************************************************************/

#pragma once
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

#include "StatusEffectTypes.generated.h"

/**
 * @enum EModifierType 
 * @see FEffectAction
 */
UENUM(BlueprintType)
enum class EModifierType : uint8
{
	Add        UMETA(DisplayName = "Add"),
	Multiply   UMETA(DisplayName = "Multiply")	
};

/**
 * @struct FEffectAction
 * @brief It is a specific value and type of modification that a status effect applies to a character's stat. 
 * For example, it can represent "Add 10 to Health" or "Multiply MoveSpeed by 0.8".
 */
USTRUCT(BlueprintType)
struct FEffectAction
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag TargetStat; // For example, Health/Speed

	UPROPERTY(EditDefaultsOnly)
	float Value;

	/**
	 * @brief Type of modification applied by this action (Add, Multiply, etc.).
	 */
	UPROPERTY(EditDefaultsOnly)
	EModifierType Type;
};


/**
 * @struct FStatusEffectDef
 * @brief Defines the specifications for a status effect.
 */
USTRUCT(BlueprintType)
struct FStatusEffectDef
{
	GENERATED_BODY()

					/* Common Spec */
	/**
	 * @brief Effect's identificator
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag EffectTag{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText Description{};

	/**
	 * @brief Duration of the effect (sec).
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = "0.1"))
	float Duration{5.f};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bRevertOnExpire{false};

	/**
	 * @brief Indicates whether the effect is periodic).
	 */
	UPROPERTY(EditDefaultsOnly)
	bool bIsPeriodic{false};

	/**
	 * @brief Time between periodic ticks (sec). 
	 */
	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = "0.1", ClampMax = "60"))
	float Period{1.f};
	
	/**
	 * @brief Maximum number of stacks for this effect.
	 */
	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = "1", ClampMax = "10"))
	int32 MaxStacks{1};

	/**
	 * @brief If true, reapplying the same effect will refresh its duration. 
	 * If false, reapplying will only increase the stack count (up to MaxStacks) without refreshing the duration.
	 */
	UPROPERTY(EditDefaultsOnly)
	bool bRefreshDurationOnReapply{true};

	/**
	 * @brief List of actions that this effect applies to the target's stats. Each action represents a specific modification (e.g., "Add 10 to Health" or "Multiply Speed by 0.8").
	 */
	UPROPERTY(EditDefaultsOnly)
	TArray<FEffectAction> Actions;

};

/**
 * @struct FActiveStatusEffect
 * @brief Represents an active instance of a status effect on a character.
 */
USTRUCT()
struct FActiveStatusEffect
{
	GENERATED_BODY()

	UPROPERTY()
	FStatusEffectDef Spec{};

	UPROPERTY()
	float RemainingTime{0.f};

	UPROPERTY()
	float TimeUntilNextTick{0.f};

	UPROPERTY()
	float TimeAccumulator{0.f};

	UPROPERTY()
	int32 StackCount{1};

	/**
	 * @brief Indicates whether the effect's actions have been applied at the start. This is used when we need to revert effects. Need to remember this for a correct revert when the duration expires.
	 */
	UPROPERTY()
	bool bAppliedOnStart{false};
};
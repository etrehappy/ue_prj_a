/*****************************************************************//**
 * \file   StatusEffectsComponent.h
 * \brief  Component that manages status effects (buffs or debuffs) for an actor.
 * 
 * \date   April 2026
 *********************************************************************/
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "StatusEffect/StatusEffectTypes.h"
#include "StatusEffect/StatusEffectData.h"
#include "Engine/AssetManager.h"

#include "StatusEffectsComponent.generated.h"

class IStatusEffectStatHandler;

/**
 * @class UStatusEffectComponent 
 * @brief 
 * 
 * @details Network: available
 * 
 * @see FStatusEffectDef
 * @see UStatusEffectData
 */
UCLASS(ClassGroup = (CustomStatus), meta = (BlueprintSpawnableComponent))
class COMMON_API UStatusEffectComponent : public UActorComponent
{
	GENERATED_BODY()

						/* === C++ member functions === */
public:
	UStatusEffectComponent();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/**
	 * @brief Server function.
	 * 
	 * Applies a status effect based on the provided definition. 
	 * @param[in] Spec The definition of the status effect to apply.
	 * @see ApplyEffectByTag
	 */
	void ApplyEffectSpec(const FStatusEffectDef* Spec);

	/**
	 * @brief Server function.
	 * 
	 * When effect hasn't duration or period, we apply its actions immediately.
	 * @see ApplyEffectSpec
	 */
	void ApplyInstantActions(FActiveStatusEffect& Effect);

	/**
	 * @brief Server function.
	 * 
	 * One effect can have multiple actions, and this function applies only one of them. It is used for both periodic and instant actions.
	 * 
	 * @see TickComponent
	 * @see ApplyInstantActions	 
	 */
	void ApplyAction(const FEffectAction& Action);

	/**
	 * @brief Server function.
	 * 
	 * Used if effect has duration and/or period. 
	 * @see TickComponent
	 * @see ApplyInstantActions	 
	 */
	void RemoveInstantActions(const FActiveStatusEffect& Effect);

	/**
	 * @brief Server function.
	 * 
	 * Removes one action, if effect has duration and/or period and bRevertOnExpire is true.
	 */
	void RemoveAction(const FEffectAction& Action);

	/**
	 * @brief  Server function.
	 * 
	 * Used for stacks.	 
	 * @return True if the effect was updated, false if the effect is not active and needs to be applied as new.
	 * 
	 */
	bool UpdateActiveEffect(const FStatusEffectDef* Spec);

private:
	/**
	 * @brief Helper function
	 * @see TickComponent	 
	 */
	void CheckActiveEffects(float DeltaTime);

	/**
	 * @brief Helper function
	 * @see ApplyAction
	 * @see RemoveAction
	 */
	bool CanHandleStatTag(UActorComponent* Component, const FEffectAction& Action, IStatusEffectStatHandler*& OutHandler) const;


						/* === Unreal Engine UFUNCTION === */

public:
	/**
	 * @brief Server and client function.
	 * 
	 * It applies a status effect to the actor based on the provided gameplay tag. 
	 * 
	 * @param[in] EffectTag the tag of buff or debuff.
	 */
	UFUNCTION(BlueprintCallable)
	void ApplyEffectByTag(FGameplayTag EffectTag);

private:
	UFUNCTION(Server, Reliable)
	void Server_ApplyEffectByTag(FGameplayTag EffectTag);


						/* === Unreal Engine UPROPERTY === */
protected:
	/**
	 * @brief If effect has duration and/or period, it is added to this array. 
	 * @see FActiveStatusEffect
	 */
	UPROPERTY()
	TArray<FActiveStatusEffect> ActiveEffects;



};
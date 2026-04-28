/*****************************************************************//**
 * \file   NpcAttackLogicBase.h
 * \brief  Base class for NPC attack logic. Provides an interface for different attack behaviors.
 * 
 * \date   April 2026
 *********************************************************************/
#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"

#include "NpcAttackLogicBase.generated.h"

class UNpcBattleComponent;

/**
 * @class UNpcAttackLogicBase
 * @brief It is neccessary to have different attack logics for different NPCs. For example, a melee NPC might have a different attack logic than a ranged NPC.
 */
UCLASS(Abstract, Blueprintable)
class NPC_API UNpcAttackLogicBase : public UObject
{
	GENERATED_BODY()

public:
	virtual void Initialize(UNpcBattleComponent* InBattleComponent);
	virtual bool CanStartAttack() const { return true; };
	virtual void OnAttackStarted() {};
	virtual void OnAttackStopped() {};
	virtual bool OnDamageWindowStarted() { return true; };
	virtual void OnDamageWindowEnded() {};

	/**
	 * @see UNpcBattleComponent::TickComponent
	 * @see WantsTick
	 */
	virtual void TickDamageWindow(float DeltaTime) {};

	/**
	 * @brief Indicates whether this attack logic requires ticking during the damage window. If true, `TickDamageWindow` will be called every tick while the damage window is active.
	 */
	virtual bool WantsTick() const { return true; };

protected:
	/**
	 * @brief Owner
	 */
	UPROPERTY()
	TObjectPtr<UNpcBattleComponent> BattleComponent{};
};
/*****************************************************************//**
 * \file   StatusEffectStatHandler.h
 * \brief  The simple implementation to handle stat modifications.
 * 
 * \date   April 2026
 *********************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "StatusEffect/StatusEffectTypes.h"

#include "StatusEffectStatHandler.generated.h"

/**
 * @interface IStatusEffectStatHandler
 * @brief Diverse components should implement this interface to be able to receive and process actions from status effects. For example, UHealthComponent will implement it to handle health modifications.
 * 
 * @see FStatusEffectDef
 * @see UStatusEffectComponent
 * @see UHealthComponent
 */
UINTERFACE()
class COMMON_API UStatusEffectStatHandler : public UInterface
{
	GENERATED_BODY()
};

class COMMON_API IStatusEffectStatHandler
{
	GENERATED_BODY()

public:
	/**
	 * @brief Checks if the component can handle a specific stat tag (for example, health, mana, stamina, etc.).
	 * 
	 * @param[in,out] StatTag The stat tag to check.
	 * @return true if the component can handle the stat tag, false otherwise.
	 */
	virtual bool CanHandleStatTag(const FGameplayTag& StatTag) const = 0;

	/**
	 * @brief Applies the specified status effect action to the component's stat.
	 * 
	 * @see FEffectAction
	 * @see FStatusEffectDef
	 */
	virtual void ApplyStatusEffectAction(const FEffectAction& Action) = 0;


	virtual void RemoveStatusEffectAction(const FEffectAction& Action) = 0;
};
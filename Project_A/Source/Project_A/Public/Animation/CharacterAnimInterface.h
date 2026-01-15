

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameplayTagContainer.h"
#include "CharacterStateEnums.h"
#include "CharacterAnimInterface.generated.h"


// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UCharacterAnimInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PROJECT_A_API ICharacterAnimInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.

public: 
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Animation") 
	void SetCanAttack(bool IsAbleToAttack);/*TODO Check Net today*/

	/* 
	* @brief Updates the attack state tags for the character's animation system.
	 * @param OutTags - Ability + weapon
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Animation")
	void UpdateAttackState(const FGameplayTagContainer& OutTags, bool IsAbleToAttack, E_CharacterBattleState BattleState);/*TODO Check Net today*/
};

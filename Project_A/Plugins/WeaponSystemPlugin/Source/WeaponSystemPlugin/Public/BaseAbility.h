/*****************************************************************//**
 * \file   BaseAbility.h
 * \brief  Base class for abilities in the weapon system.
 * 
 * \date   January 2026
 *********************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "InputTriggers.h"
#include "InputAction.h"
#include "BaseAbility.generated.h"

class UAbilityComponent;

/**
 * @class UBaseAbility
 * @brief Ability manages specific actions that a character can perform. 
 * @see UAbilityComponent
 * @todo Should it know about AbilityComponent?
 */
UCLASS(Abstract, Blueprintable)
class WEAPONSYSTEMPLUGIN_API UBaseAbility : public UObject
{
	GENERATED_BODY()

						/* === C++ member functions === */
public:	
	UBaseAbility() = default;
	virtual ~UBaseAbility() = default;
		
	/**
	 * @brief It is used in AbilityComponent to initialize ability
	 * @see UAbilityComponent::CreateAbilities
	 * @todo Check network.	Should it be called on the server only?
	 */
	void InitAbility(UAbilityComponent* Owner);

	/**
	 * @brief Server function.
	 * @param[in] TriggerEvent
	 */
	void HandleInput(const ETriggerEvent TriggerEvent);

	FGameplayTag GetAbilityTag() const;
	FName GetInputActionName() const;

protected:
	/**
	 * @brief Server function.
	 * 
	 * The RequiredWeaponTags property of an ability asset indicates which weapon type is required.
	 * @return true, if a weapon currently has at least one tag from the RequiredWeaponTags list.
	 */
	bool HasOwnerRequiredWeapon() const;

private:	
	/**
	 * @brief Server function.
	 * 
	 * It broadcasts OnAbilityActivated delegate in AbilityComponent.
	 */
	void Activate() const;

						/* === C++ member variables === */

private:
	/**
	 * @brief In the current implementation, an ability knows which input action it is bound to.
	 * It sets in InitAbility function from InputActionObjForName property.
	 */
	FName InputAction{}; // "AttackLight", "AttackHeavy", "BowShot" /*TODO Check Net today*/
		

						/* === Unreal Engine UFUNCTION === */
protected:
	
	/**
	 * @brief Server function.
	 * @see HandleInput
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Ability")
	void OnInputStarted();

	/**
	 * @brief Server function.
	 * @see HandleInput
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Ability")
	void OnInputTriggered();

	/**
	 * @brief Server function.
	 * @see HandleInput
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Ability")
	void OnInputOngoing();

	/**
	 * @brief Server function.
	 * @see HandleInput
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Ability")
	void OnInputCanceled();

	/**
	 * @brief Server function.
	 * @see HandleInput
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Ability")
	void OnInputCompleted();


						/* === Unreal Engine UPROPERTY === */
protected:
	/**
	 * @todo Check network. Should it be replicated?
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability")
	FGameplayTag AbilityTag{}; 

	/**
	 * @brief sword, bow, staff
	 * @todo Check network. Should it be replicated?
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability")
	FGameplayTagContainer RequiredWeaponTags{}; 

	/**
	 * @brief It is set in InitAbility function.
	 * @todo Check network. Should it be replicated?
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Ability")
	TObjectPtr<UAbilityComponent> OwnerAbilityComponent{};

private:	
	/**
	 * @brief This is used only to set FName InputAction
	 * @see UBaseAbility::InputAction
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability", meta = (DisplayName = "Input Action", AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> InputActionObjForName{};
  
};


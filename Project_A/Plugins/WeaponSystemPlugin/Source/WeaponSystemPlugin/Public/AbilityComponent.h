/*****************************************************************//**
 * \file   AbilityComponent.h
 * \brief  The general ability component for a player character.
 * 
 * \date   January 2026
 *********************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "BaseAbility.h"
#include "InputTriggers.h"

#include "AbilityComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAbilityActivated, FGameplayTag, AbilityTag);

/**
 * @class UAbilityComponent
 * @brief It manages a collection of abilities. In the current implementation, a character has UAbilityComponent and UWeaponComponent for managing combat.
 * Check ABaseAbility for more details about abilities.
 */
UCLASS(Blueprintable, ClassGroup=(Ability), meta=(BlueprintSpawnableComponent) )
class WEAPONSYSTEMPLUGIN_API UAbilityComponent : public UActorComponent
{
	GENERATED_BODY()

						/* === C++ member functions === */
public:	
	UAbilityComponent();
    virtual ~UAbilityComponent() = default;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/**
	 * @brief Calls server function to handle input action.
	 * 
	 * @param[in] InputName
	 * @param[in] Event
	 * @see UCustomInputComponent::HandleAbility
	 */
	void HandleInputAction(const FName InputName, const ETriggerEvent Event) const;

protected:
	virtual void BeginPlay() override;

private:
	/**
	 * @brief Creates instances of abilities based on the AbilityClasses array.
	 * @todo Check network.	Should it be called on the server only?
	 */
    void CreateAbilities();


						/* === Unreal Engine UFUNCTION === */
private:
	/**
	 * @brief If an ability is found for the input action, the ability's handler will be called.
	 * 
	 * @param[in] InputName
	 * @param[in] Event
	 */
	UFUNCTION(Server, Reliable)
	void Server_HandleInputAction(const FName InputName, const ETriggerEvent Event) const;


						/* === Unreal Engine UPROPERTY === */
public:

	/**
	 * @brief Broadcasts when an ability is activated.
	 * It is used in PlayerCharacter to start attack
	 */
	UPROPERTY(BlueprintAssignable, Category = "Abilities")
	FOnAbilityActivated OnAbilityActivated{};

protected:
	/**
	 * @brief It sets up in Blueprint.
	 * @todo Check network. Should it be replicated?
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities")
	TArray<TSubclassOf<UBaseAbility>> AbilityClasses{};

private:
	/**
	 * @brief It is instances of abilities.
	 * @todo Check network. Should it be replicated?
	 */
	UPROPERTY()
	TArray<TObjectPtr<UBaseAbility>> Abilities{};
};


//UFUNCTION(BlueprintCallable)
//void ActivateAbilityByTag(FGameplayTag AbilityTag);

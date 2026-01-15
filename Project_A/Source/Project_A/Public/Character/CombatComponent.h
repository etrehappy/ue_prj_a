/*****************************************************************//**
 * \file   CombatComponent.h
 * \brief  The general combat component for a player character.
 * 
 * \date   January 2026
 *********************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "CharacterStateEnums.h"
#include "Animation/CombatMontageTable.h"
#include "WeaponComponent.h"
#include "Character/CustomInputComponent.h"

#include "CombatComponent.generated.h"

class ANetPlayerCharacter;
class UCustomLocomotionComponent;

/**
 * @brief It manages states and actions related to combat for a character. 
 * In current implementation, it has hard-link with UWeaponComponent.
 * 
 * @todo Check whether it should know about UWeaponComponent.
 */
UCLASS( ClassGroup=(CustomCombat), meta=(BlueprintSpawnableComponent) )
class PROJECT_A_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()
						/**  === C++ member functions === */
public:	
	UCombatComponent();
	virtual ~UCombatComponent() = default;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	/**
	 * @brief Updates internal pointers to the owner character and its weapon component.
	 */
	void Initialize();

public:
	/**
	 * @brief Server function.
	 * 
	 * It updates combat states of the character. 
	 * In the current implementation, this step is important for animations, although it does not manage them directly.
	 * 
	 * @param[in] CurrentAbilityTag An ability, which was just activated.
	 * @param[in] CurrentWeaponTag A weapon, which is currently equipped.
	 * @return true, if the attack state was changed; false otherwise.
	 * @see CurrentAttackTags
	 * @see ANetPlayerCharacter::HandleAbilityActivated
	 * @see UAbilityComponent
	 * @see AWeaponBase
	 * 
	 * @todo UCombatComponent must be subsribed OnWeaponChanged
	 */
	virtual bool UpdateAttackState(const FGameplayTag& CurrentAbilityTag, const FGameplayTag& CurrentWeaponTag);

	/**
	 * @brief Client
	 * 
	 * This is used to check whether a weapon currently in use must be unequipped. If so, it will be unequipped.
	 * In the current implementation, the same button is used for both equipping and unequipping a weapon.
	 * 
	 * @param[in] InputActionType The logic is split between weapons and throwable items.
	 * @return true, if a new weapon will be equipped; false, if the current weapon will be unequipped.
	 * @see LocomotionComponent must be subsribed OnEquiped/Unequiped
	 * @see ANetPlayerCharacter::EquipWeapon
	 */
	bool TryUnequipWeapon(const EInputActionId InputActionType);

	/**
	 * @brief Client function.
	 *
	 * Sends RPC to the server for stop the current attack.
	 * @see UAttackFinished::Notify
	 */
	void OnAttackAnimationFinished();

	/**
	 * @brief Client function.
	 *
	 * Spawns a projectile on the client side. In the current implementation, the server does not spawn it, but the server validates a hit.
	 * @see UAnimNotify_SpawnMagicProjectile
	 */
	void OnSpawnProjectile();

	/**
	 * @brief Client function.
	 *
	 * Spawns a projectile on the client side. In the current implementation, the server does not spawn it, but the server validates a hit.
	 * @todo Is it possible to combine it with OnSpawnProjectile?
	 * @see UAnimNotify_SpawnBombProjectile
	 *
	 */
	void OnSpawnBombProjectile();

protected:
	virtual void BeginPlay() override;

private:
	/**
	 * @brief Client function.
	 * 
	 * If a battle state is changed to 'Attack', it will find and play the corresponding montage.
	 * @see OnRep_UpdateAttackState
	 */
	void StartAnimAttack();
	
	/**
	 * @brief Client function.
	 * @see StartAnimAttack
	 */
	UAnimMontage* FindMontage();

	/**
	 * @brief Client and server function.
	 * @return The animation instance of the owner character's skeletal mesh component.
	 */
	UAnimInstance* GetAnimInstance() const;

	/**
	 * @brief Client function.
	*/
	void CleanCurrentMontage();

						
						/** === C++ member variables === */
private:
	/**
	 * @brief It is cached for cleaning montage as required.
	 */
	UAnimMontage* CurrentMontage{nullptr};


						/** === Unreal Engine UFUNCTION === */

protected:	
	UFUNCTION(Server, Reliable)
	void Server_StopAttack();

	/**
	 * @brief Updates combat states on clients to stop the current attack.
	 */
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_StopAttack();

	/**
	 * @brief Called on clients when CurrentAttackTags is updated on the server.
	 * @see CurrentAttackTags
	 */
	UFUNCTION()
	void OnRep_UpdateAttackState();


						/** === Unreal Engine UPROPERTY === */
protected:
	/**
	 * @brief It is a main parameter of this component.
	 * It contains an ability tag and a weapon tag. Together, they define the current attack state and the corresponding animation montage.
	 */
	UPROPERTY(VisibleDefaultsOnly, ReplicatedUsing = OnRep_UpdateAttackState, BlueprintReadOnly, Category = "Attack")
	FGameplayTagContainer CurrentAttackTags{};

	/**
	 * @brief Replication are handled manually. It should only be replicated when the value of CurrentAttackTags changes.
	 * 
	 * It is used in ServerUpdateAttackState, OnRep_UpdateAttackState, Multicast_StopAttack
	 * @todo Is it necessary to replicate it manually? Could CurrentAttackTags be used instead?
	 */
	UPROPERTY(VisibleDefaultsOnly, /*Replicated,*/ BlueprintReadOnly, Category = "Attack")
	bool bIsFighting{false};

	/**
	 * @brief In the current implementation, it is used only with throwable items in UpdateAttackState.
	 * @todo Does this make sense? Used only in UpdateAttackState.
	 * @todo Check all usings
	 */
	UPROPERTY(VisibleDefaultsOnly, Replicated, BlueprintReadOnly, Category = "Attack")
	E_CharacterBattleState BattleState{E_CharacterBattleState::Normal};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack")
	TObjectPtr<UDataTable> MontageTable{};

private:
	UPROPERTY()
	TObjectPtr<ANetPlayerCharacter> OwnerCharacter{};

	/**
	 * @brief A WeaponComponent of the owner character.
	 */
	UPROPERTY()
	TObjectPtr<UWeaponComponent> OwnerWeaponComponent{};
};



///**
// * @see BattleState
// */
//E_CharacterBattleState GetBattleState();
//void ChangeWeapon();

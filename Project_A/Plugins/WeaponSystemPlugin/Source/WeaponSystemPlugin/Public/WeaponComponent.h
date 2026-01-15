/*****************************************************************//**
 * \file   WeaponComponent.h
 * 
 * \date   January 2026
 *********************************************************************/

#pragma once

#include "CoreMinimal.h"

#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "WeaponBase.h"
#include "WeaponComponent.generated.h"

class AWeaponThrowable;

/**
 * @struct FLustWeaponSettings
 * @brief This is used to restore the last weapon.
 */
USTRUCT()
struct FLustWeaponSettings
{
	GENERATED_BODY();

public:
	void Update(const FGameplayTag WeaponTag, const FName AttachSocketName, const FTransform& SpawnTransform, ACharacter* AttachTo = nullptr, bool bIsCollisionDesabled = true, FName ProjectileSocketName = "");
	bool IsEmpty() const;

	FGameplayTag WeaponTag_m{};
	FName AttachSocketName_m{};
	FTransform SpawnTransform_m{};
	ACharacter* AttachTo_m{};
	bool bIsCollisionDesabled_m{};
	FName ProjectileSocketName_m{};
};

/**
 * @class UWeaponComponent
 * @brief The general weapon component for a player character.
 * @details Responsibilities:
 * - Managing any weapons (melee, throwable, magic, etc.) for a character
 * - Equipping and unequipping weapons and throwable items.
 * - Handling attack actions and interactions with the equipped weapons.
 * - Providing access to the current weapon and its properties.
 * 
 * Network: available
 * 
 * @see AWeaponBase
 * @see AAbilityComponent
 */
UCLASS( ClassGroup=(Weapon), meta=(BlueprintSpawnableComponent) )
class WEAPONSYSTEMPLUGIN_API UWeaponComponent : public UActorComponent
{
	GENERATED_BODY()
						/** === C++ member functions === */
public:	
	UWeaponComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/**	
	 * @brief Server and Client function.
	 * 
	 * @return true, if a throwable item is held in the hand.
	 * @see CurrentThrowableItem
	 */
	const bool IsCurrentThrowableItemEquiped() const;

	/** 
	 * @brief Server and Client function.
	 * @return true, if a weapon (magic or melee, for example, but not throwable weapons) is held in the hand.
	 * @see CurrentWeapon
	 */
	const bool IsCurrentWeaponEquiped() const;

	/**
	 * @brief Server function.
	 * 
	 * @return CurrentThrowableItem or CurrentWeapon if any of them is equiped, otherwise nullptr.
	 * @see CurrentThrowableItem CurrentWeapon
	 */
	AWeaponBase* GetAnyActiveEquipedWeapon() const;

	/**
	 * @brief Server and Client function.
	 * @return CurrentWeapon
	 */
	AWeaponBase* GetCurrentWeapon() const;

	/**
	 * @brief Server and Client function.
	 * @return CurrentThrowableItem
	 */
	AWeaponThrowable* GetCurrentThrowableItem() const;

	/**
	 * @brief Client function.
	 *   
	 * @return ProjectileSocketName
	 */
	FName GetProjectileSocketName() const;

	/**
	 * @brief Server function.
	 *
	 * Checks whether a weapon or throwable item is equipped and calls the relevant attack function.
	 * @see UCombatComponent::ServerUpdateAttackState
	 */
	void Attack();

	/**
	 * @brief Server function.
	 * 
	 * Checks whether a weapon or throwable item is equipped and calls the relevant stop attack function.
	 * If weapon is a throwable item, it unequips it after stopping the attack.
	 * @see UCombatComponent::Server_StopAttack_Implementation
	 */
	void StopAttack();

	/** 
	 * @brief Client and server function.
	 * 
	 * It calls a server RPC to unequip the current weapon.
	 * @see Server_UnequipWeapon_Implementation
	 */
	void UnequipWeapon();

	/**
	 * @brief Client and server function.
	 * 
	 * It calls a server RPC to unequip the current throwable item.
	 * @see	Server_UnequipThrowableItem_Implementation
	 */
	void UnequipThrowableItem();
	

protected:
	virtual void BeginPlay() override;	

private:
	/**
	 * @brief Server function.
	 * 
	 * Updates the CurrentWeapon to NewWeapon and attaches it to the specified socket on the character.
	 * @see CurrentWeapon
	 * @see Server_EquipWeaponByTag_Implementation
	 */
	void SetCurrentWeapon(AWeaponBase* NewWeapon, const FGameplayTag WeaponTag, const FName AttachSocketName, const FTransform& SpawnTransform, ACharacter* AttachTo = nullptr, bool bIsCollisionDesabled = true, FName ProjectileSocketName = "");

	/**
	 * @brief Server function.
	 * 
	 * Updates the CurrentThrowableItem to InThrowableItem and attaches it to the specified socket on the character.
	 * @see CurrentThrowableItem
	 * @see Server_EquipWeaponByTag_Implementation
	 */
	void SetCurrentThrowableItem(AWeaponThrowable* InThrowableItem, USceneComponent* AttachTo, bool bIsCollisionDesabled, const FName AttachSocketName, FName InProjectileSocketName);

	/**
	 * @brief Server function.
	 * 
	 * If a weapon was equipped before a throwable item, this function puts it back in the character's hand.
	 * @see LastWeaponSettings
	 * @see Server_UnequipThrowableItem_Implementation
	 * @see Server_EquipWeaponByTag_Implementation
	 */
	void PutLastWeaponBackOn();


						/** === C++ member variables === */
private:
	/**
	 * @brief Server only.
	 * 
	 * Used to restore the last weapon if equipping fails or a throwable item is unequipped.
	 * @see PutLastWeaponBackOn
	 */
	FLustWeaponSettings LastWeaponSettings{};

						/** === Unreal Engine UFUNCTION === */	
public:
	/**
	 * @brief Client function for weapons and throwable items.
	 * 
	 * It is called from Blueprint to equip a weapon using its gameplay tag.
	 * 
	 * @param[in] WeaponTag The gameplay tag identifying the weapon (including throwable items) to equip.
	 * @param[in] AttachSocketName The name of the socket to attach the weapon to. It can be empty, but not recommended.
	 * @param[in] SpawnTransform The transform to use when spawning the weapon.
	 * @param[in] AttachTo The skeletal mesh of the character who will hold the weapon. If AttachTo is empty, it returns without equipping.
	 * @param[in] bIsCollisionDesabled Whether collision should be disabled for the weapon.
	 * @param[in] InProjectileSocketName The name of the socket for projectiles, if applicable.
	 * 
	 * @see Server_EquipWeaponByTag
	 * @todo Check whether bIsCollisionDesabled needs to be used.
	 */
	UFUNCTION(BlueprintCallable)
	void EquipWeaponbyTag(const FGameplayTag WeaponTag, const FName AttachSocketName, const FTransform& SpawnTransform, USkeletalMeshComponent* AttachTo = nullptr, bool bIsCollisionDesabled = true, FName InProjectileSocketName = "");			

protected:
	UFUNCTION(Server, Reliable)
	void Server_UnequipWeapon();

	UFUNCTION(Server, Reliable)
	void Server_UnequipThrowableItem();

	/**
	 * @brief Gets all parameters from EquipWeaponbyTag (Blueprint) and executes the equipping logic on the server.
	 * 
	 * @see PutLastWeaponBackOn EquipWeaponbyTag
	 * @see SetCurrentWeapon SetCurrentThrowableItem
	 */
	UFUNCTION(Server, Reliable)
	void Server_EquipWeaponByTag(const FGameplayTag WeaponTag, const FName AttachSocketName, const FTransform SpawnTransform, ACharacter* AttachToCharacter, bool bIsCollisionDesabled = true, FName InProjectileSocketName = "");

						/** === Unreal Engine UPROPERTY === */
private:
	/**
	 * @brief Any weapon (magic, melee, etc., but not throwable items) currently held by the character.
	 * If CurrentThrowableItem is equipped, this will be nullptr.
	 * @see LastWeaponSettings
	 */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Replicated, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<AWeaponBase> CurrentWeapon{};

	/**
	 * @brief Any throwable item (grenades, bombs, etc.) currently held by the character.
	 * If CurrentWeapon is equipped, this will be nullptr.
	 */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Replicated, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<AWeaponThrowable> CurrentThrowableItem{};

	/**
	 * @brief Used for spawning projectiles from the equipped weapon or throwable item, if applicable.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Replicated, meta = (AllowPrivateAccess = "true"))
	FName ProjectileSocketName{};

	/**
	 * @brief Indicates whether a weapon (magic, melee, etc., but not throwable items) is currently equipped.
	 * @see CurrentWeapon
	 */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Replicated, meta = (AllowPrivateAccess = "true"))
	bool bIsWeaponEquiped{false};

	/**
	 * @brief Indicates whether a throwable item (grenades, bombs, etc.) is currently equipped.
	 * @see CurrentThrowableItem
	 */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Replicated, meta = (AllowPrivateAccess = "true"))
	bool bIsThrowableItemEquiped{false};
	
};

//	const bool IsAnyWeaponEquiped() const;
// void CleanThrowableItem();/*TODO Check Net*/
	//UFUNCTION(BlueprintCallable)
	//AWeaponBase* EquipWeaponbyClass(TSubclassOf<AWeaponBase> WeaponClass);
//UFUNCTION(BlueprintCallable)
//void Reload() {};/*TODO Check Net*/

//	/**
//	 * @brief The current implementation only updates bIsWeaponEquipped.
//	 * @see bIsWeaponEquiped
//	 */
//UFUNCTION()
//void OnRep_CurrentWeapon();

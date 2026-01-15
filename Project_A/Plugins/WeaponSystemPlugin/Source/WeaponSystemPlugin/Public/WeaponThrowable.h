/*****************************************************************//**
 * \file   WeaponThrowable.h
 * \brief  It is the base class for throwable weapons (grenades, bombs, etc.).
 * 
 * \date   January 2026
 *********************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponBase.h"


#include "WeaponThrowable.generated.h"

class ATargetIndicator;
class AAreaExplosion;

/**
 * @class AWeaponThrowable
 * @brief A throwable item has two states: before throwing (aiming) and after throwing (attack).
 * It implements logic for spawning a projectile and showing an aiming indicator.
 * @see AProjectileBase ATargetIndicator
 * @todo Check static variables.
 */
UCLASS()
class WEAPONSYSTEMPLUGIN_API AWeaponThrowable : public AWeaponBase
{
	GENERATED_BODY()
	
						/** === C++ member functions === */
public:	
	AWeaponThrowable();
	virtual ~AWeaponThrowable() override = default;	
	virtual void StopAiming() override;
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/**
	 * @brief Client function.
	 * 
	 * If a projectile is spawned, the throwable item will hide its mesh. 
	 * @see UCombatComponent::OnSpawnBombProjectile
	 */
	virtual void SpawnProjectile() override;

	/**
	 * @see UWeaponComponent::SetCurrentThrowableItem
	 */
	virtual void StartAiming() override;

	/**
	 * @brief Server function.
	 *
	 * A throwable item is destroyed after an animation has finished.
	 * @see SpawnProjectile
	 */
	
	bool IsAiming() const;

protected:
	virtual void BeginPlay() override;

private:
	/**
	 * @brief Client function.
	 * 
	 * Spawns the aiming indicator if it does not exist yet.
	 * @see ATargetIndicator
	 * @todo It should not use socket names directly.
	 */
	void SpawnIndicatorIfNeeded();

	/**
	 * @brief Client function.
	 * 
	 * Updates the aiming indicator's position based on the current ballistic data.
	 * @see ATargetIndicator::UpdateIndicator
	 */
	void UpdateIndicator();

	/**
	 * @brief Server function.
	 * 
	 * Executes the throwing action on the server.
	 * @param TargetServer The target location where the projectile should be thrown.
	 */
	void ExecuteFire(const FVector& TargetServer);	


						/** === Unreal Engine UFUNCTION === */
protected:
	/**
	 * @brief If a player inputs an attack command, this client function is called to throw an item.
	 */
	void Client_StartAttack_Implementation() override;

	/**
	 * @brief The server needs to validate the throw, so updating ballistic data is necessary.
	 */
	UFUNCTION(Server, Unreliable)
	void Server_UpdateBallisticData(FVector InCachedStartLocation, FVector InCachedInitialVelocity,
		FVector InLastAimLocation, bool bInIsLastAimValid);

	UFUNCTION(Server, Reliable)
	void Server_SetIsAiming(bool bNewIsAiming);

	UFUNCTION(Server, Reliable)
	void Server_RequestFireAtLocation(const FVector& Target);

	UFUNCTION(Client, Reliable)
	virtual void Client_StartAiming();

	UFUNCTION(Client, Reliable)
	virtual void Client_StopAiming();

private:
	/**
	 * @brief The radius of the explosion caused by the throwable item.
	 * @todo Should it be a part of the projectile class?
	 */
	UPROPERTY(EditAnywhere, /*Replicated,*/ Category = "Weapon|Bomb")
	float ExplosionRadius{300.f};

	/**
	 * @brief Initial speed of the thrown projectile.
	 * @todo Should it be a part of the projectile class?
	 */
	UPROPERTY(EditAnywhere, /*Replicated,*/ Category = "Weapon|Bomb")
	float InitialSpeed{2000.f};

	/**
	 * @todo Should it be a part of the projectile class?
	 */
	UPROPERTY(EditAnywhere, /*Replicated,*/	Category = "Weapon|Bomb")
	float GravityZ{-980.f};

	/**
	 * @brief The aiming indicator used to show the target location.
	 */
	UPROPERTY(/*Replicated*/)
	ATargetIndicator* Indicator{};

	UPROPERTY(Replicated)
	bool bIsAiming{false};

	UPROPERTY(/*Replicated*/)
	FVector CachedStartLocation{};

	UPROPERTY(/*Replicated*/)
	FVector CachedInitialVelocity{};

	UPROPERTY(/*Replicated*/)
	FVector LastAimLocation{};

	UPROPERTY(/*Replicated*/)
	bool bIsLastAimValid{false};

};

//FVector GetLastAimLocation() const;
//bool IsLastAimValid() const;

/*****************************************************************//**
 * \file   ProjectileBase.h
 * \brief  Base class for projectiles in the weapon system.
 * 
 * \date   January 2026
 *********************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AreaExplosion.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "WspDamageType.h"

#include "ProjectileBase.generated.h"

/**
 * @class AProjectileBase
 * @brief 
 * @see AAreaExplosion
 * @see AWeaponThrowable AWeaponMagic
 */
UCLASS()
class WEAPONSYSTEMPLUGIN_API AProjectileBase : public AActor
{
	GENERATED_BODY()

						/* === C++ member functions === */
public:		
	AProjectileBase();
	virtual ~AProjectileBase() = default;
	virtual void Tick(float DeltaTime) override;
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	USphereComponent* GetCollisionComponent() const;
	UProjectileMovementComponent* GetMovementComponent() const;
	
	virtual void NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;

	/**
	 * @brief Server function.
	 * 
	 * Initialize projectile movement and physics parameters after spawn. In the current implementation, it is called only in the AWeaponThrowable.
	 * @param LaunchVelocity World-space initial velocity for the projectile.
	 * @param GravityZOverride Gravity (world units) to be used for this projectile (can be negative).
	 * @todo Should this be a general mechanism for all projectiles?
	 */
	void InitializeFromLaunch(const FVector & LaunchVelocity, float GravityZOverride = 0.0f);

protected:
	virtual void BeginPlay() override;

private:
	/**
	 * @brief Server support function.
	 * 
	 * @see NotifyHit
	 */
	void Explode(const FHitResult& Hit);


	/* === Unreal Engine UFUNCTION === */
public:
	/**
	 * @brief Can be used in Blueprints to implement custom behavior upon projectile impact.
	 * 
	 * @param[in,out] Hit
	 */
	UFUNCTION(BlueprintImplementableEvent)
	void OnProjectileImpact(const FHitResult& Hit);


	/* === Unreal Engine UPROPERTY === */
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USphereComponent> Collision{};
		
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	float Damage{20.f};

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UProjectileMovementComponent> Movement{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated, Category = "Weapon")
	TSubclassOf<AAreaExplosion> ExplosionClass{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated, Category = "Weapon")
	float ExplosionRadius {300.f};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	bool bIsItAoe{false};

	UPROPERTY(EditAnywhere, Category = "Weapon")
	TSubclassOf<UWspDamageType> DamageTypeClass;

};

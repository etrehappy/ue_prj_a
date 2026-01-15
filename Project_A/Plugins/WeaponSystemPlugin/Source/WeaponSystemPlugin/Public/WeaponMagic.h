/*****************************************************************//**
 * \file   WeaponMagic.h
 * \brief  In the current implementation, this weapon only spawns projectiles.
 * 
 * \date   January 2026
 *********************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponBase.h"

#include "WeaponMagic.generated.h"

/**
 * @class AWeaponMagic
 * @brief 
 * @ todo Update the static variables in the .cpp file. These should be configurable.
 */
UCLASS()
class WEAPONSYSTEMPLUGIN_API AWeaponMagic : public AWeaponBase
{
	GENERATED_BODY()

public:
	AWeaponMagic();
	virtual ~AWeaponMagic() = default;
	virtual void Tick(float DeltaTime) override;

	/**
	 * @brief Client function.
	 * 
	 * Spawns a projectile for the magic weapon.
	 * The class of a projectile is set up via the WeaponDataAsset.	
	 */
	virtual void SpawnProjectile() override;

protected:
	virtual void BeginPlay() override;

	UFUNCTION(Server, Reliable)
	void Server_SpawnProjectile(const FVector AimOrigin, const FVector AimDirection);

private:
	// Support functions for spawning projectiles
	bool GetPlayerAim(FVector& OutWorldOrigin, FVector& OutWorldDirection) const;
	FVector ComputeTraceEnd(const FVector& WorldOrigin, const FVector& WorldDirection) const;
	bool PerformAimTrace(const FVector& WorldOrigin, const FVector& TraceEnd, FHitResult& OutHit) const;
	/**
	 * @brief In the current implementation, this gets the spawn transform with socket, but ABP doesn't work on the server.
	 * @todo it should be client function 
	 */
	bool GetProjectileSpawnTransform(FVector& OutLocation, FRotator& OutRotation, FVector& OutDirection, const FVector& TargetPoint) const;
	void SpawnProjectileAt(const FVector& Location, const FRotator& Rotation, const FVector& Direction);	
	//////////////////////////////////////////////////////////////////////////


};

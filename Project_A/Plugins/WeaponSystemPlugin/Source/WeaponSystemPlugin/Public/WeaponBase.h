/*****************************************************************//**
 * \file   WeaponBase.h
 * \brief  Base class for all weapons in the Weapon System Plugin.
 * 
 * \date   January 2026
 *********************************************************************/

#pragma once

#include "CoreMinimal.h"

#include "Components/MeshComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"
#include "GameplayTagContainer.h"
#include "ProjectileBase.h"
#include "WspDamageType.h"
#include "TargetIndicator.h"

#include "WeaponBase.generated.h"

class UWeaponDataAsset;

/**
 * @class AWeaponBase
 * @brief Abstract class for all weapons in the Weapon System Plugin.
 * @details Responsibilities:
 * - Serve as a base class for different weapon types (melee, magic, throwable).
 * - Define common properties and methods for weapon behavior.
 * - Facilitate weapon initialization from data assets.
 * - Manage weapon mesh and collision settings.
 * 
 * Network: available
 * 
 * @see UWeaponFactory UWeaponDataAsset
 * @see AWeaponMelee AWeaponMagic AWeaponThrowable
 */
UCLASS(Abstract)
class WEAPONSYSTEMPLUGIN_API AWeaponBase : public AActor
{
	GENERATED_BODY()
						/* === C++ member functions === */
public:
	AWeaponBase();	
	virtual ~AWeaponBase() = default;	
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/**
	 * @brief Server function.
	 * 
	 * Initialises the mesh component using the specified mesh asset.
	 * 
	 * @param[in] NewMesh This can be either static or skeletal.
	 * @return true if the mesh component was successfully initialized.
	 */
	bool InitialiseMeshComponent(UStreamableRenderAsset* NewMesh);

	/**
	 * @brief Server function.
	 * 
	 * Once the DataAsset has been validated, the weapon's properties are initialised based on the data asset provided.
	 * 
	 * @param[in] Data The data asset containing weapon properties.
	 * @see UWeaponDataAsset
	 */
	virtual void InitialiseFromData(UWeaponDataAsset* Data);
		
	/**
	 * @brief Server function.
	 * 
	 * A simple public function for calling client-RPC.
	 */
	virtual void StartAttack();

	/**
	 * @brief Server function.
	 * 
	 * A simple public function for calling client-RPC.
	 */
	virtual void StopAttack();		  

	/**
	 * @brief Server function.
	 * 
	 * This is a specific function for weapons that support aiming. Implementation depends on a derived class.
	 */
	virtual void StartAiming() {};

	/**
	 * @brief Server function.
	 * 
	 * This is a specific function for weapons that support aiming. Implementation depends on a derived class.
	 */
	virtual void StopAiming() {};

	/**
	 * @brief Client function.
	 * 
	 * This is a specific function for weapons that support aiming. Implementation depends on a derived class.
	 * @param[in] Target The target location where the player is aiming.
	 */
	virtual void RequestFireAtLocation(const FVector& Target) {};

	/**
	 * @brief Client function.
	 *
	 * This should be overridden in derived classes in order to implement specific projectile-spawning logic.
	 * @see UAnimAttackNotifies.h
	 */
	virtual void SpawnProjectile() {};

	/**
	 * @brief Client and Server function.	 	 
	 */
	const FGameplayTag& GetWeaponTag() const;
	
	/**
	 * @brief Client and Server function.
	 */
	bool IsWeaponAttacking() const;

	/**
	 * @brief Client and Server function.
	 */
	void SetIsWeaponAttacking(bool bNewIsWeaponAttacking);

	/**
	 * @brief Server function.
	 */
	void SetCollision(ECollisionEnabled::Type CollisionEnabled, ECollisionResponse CollisionResponse);

protected:	
	virtual void BeginPlay() override;
						
	
						/* === C++ member variables === */
private:
	/**
	 * @brief Used becuse of different types of meshes.
	 */
	USceneComponent* RootSceneComponent{};

	/**
	 * @brief If the weapon has a static mesh, then the StaticMeshComponent would be used and the SkeletalMeshComponent would be destroyed.
	 * @see AWeaponBase::MeshComponent
	 * @see InitialiseMeshComponent
	 */
	UStaticMeshComponent* StaticMeshComponent{};

	/**
	 * @brief If the weapon has a skeletal mesh, then the SkeletalMeshComponent would be used and the StaticMeshComponent would be destroyed.
	 * @see AWeaponBase::MeshComponent
	 * @see InitialiseMeshComponent
	 */
	USkeletalMeshComponent* SkeletalMeshComponent{};


						/* === Unreal Engine UFUNCTION === */
protected:
	UFUNCTION(NetMulticast, Reliable)
	virtual void Multicast_SetCollision(ECollisionEnabled::Type CollisionEnabled, ECollisionResponse CollisionResponse);

	/**
	 * @brief 
	 * @see AWeaponComponent
	 * @todo Check the architecture. Decide whether the server should call RPC when a cheracter is attacked.
	 */
	UFUNCTION(Client, Reliable)
	virtual void Client_StartAttack();

	UFUNCTION(Client, Reliable)
	virtual void Client_StopAttack();
	
	UFUNCTION(Server, Reliable)
	void Server_SetIsWeaponAttacking(bool bNewIsWeaponAttacking);

	
						/* === Unreal Engine UPROPERTY === */
protected:
	/**
	 * @brief Indicates whether the weapon is currently attacking.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated, Category = "Weapon")
	bool bIsWeaponAttacking{false};

	/**
	 * @brief The character that owns this weapon.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated, Category = "Weapon")
	TObjectPtr<ACharacter> OwningCharacter{};

	/**
	 * @brief A unique tag that identifies the weapon type. Throwable items are also considered weapons.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated, Category = "Weapon")
	FGameplayTag WeaponTag{};

	/**
	 * @brief It can be either a static mesh or a skeletal mesh, depending on the weapon type.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated, Category = "Weapon")
    TObjectPtr<UMeshComponent> MeshComponent{}; // UMeshComponent - abstract base class for mesh components

	/**
	 * @brief Current implementation does not support Damage. Future versions may include it.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Weapon")
	float Damage{1.f};

	/**
	 * @brief Current implementation does not support Damage. Future versions may include it.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Weapon")
	float AttackRange{50.f};

	/**
	 * @brief The class of the projectile that this weapon can spawn. Used by magic and throwable weapons.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated, Category = "Weapon")
	TSubclassOf<AProjectileBase> ProjectileClass{};


	/**
	 * @brief Used by throwable weapons.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated, Category = "Weapon")
	TSubclassOf<ATargetIndicator> IndicatorClass{};  

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated, Category = "Weapon")
	TSubclassOf<UWspDamageType> DamageTypeClass{};

};





//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated, Category = "Weapon")
//TSubclassOf<AAreaExplosion> ExplosionClass{};

//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
//float AttackRate{};

//UFUNCTION(Server, Reliable)
//void Server_InitializeFromData(UWeaponDataAsset* Data);

//UPROPERTY(VisibleAnywhere, Replicated, Category = "Weapon")
//TSubclassOf<UAnimInstance> WeaponAnimBPClass{};

//bool DoesSupportAiming() const { return bDoesSupportAiming; } /*TODO Check Net today*/
//bool bDoesSupportAiming{false}; /*TODO Check Net*/
//
//UFUNCTION(NetMulticast, Reliable)
//void Multicast_SetMesh(UStreamableRenderAsset* NewMesh);

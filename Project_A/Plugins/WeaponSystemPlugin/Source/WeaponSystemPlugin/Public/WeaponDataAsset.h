/*****************************************************************//**
 * \file   WeaponDataAsset.h
 * \brief  The general data asset for weapons.
 * 
 * \date   December 2025
 *********************************************************************/

#pragma once

#include "CoreMinimal.h"

#include "Engine/DataAsset.h"
#include "ProjectileBase.h"
#include "WspDamageType.h"
#include "WeaponBase.h"

#include "WeaponDataAsset.generated.h"

class AAreaExplosion;
class ATargetIndicator;

/**
 * @class UWeaponDataAsset
 * @brief It used by weapon factory to spawn weapons with specific properties. The mechanic is based on Data-Driven Design.
 * Used in Blueprints to set up different types of weapons with unique attributes and behaviors.
 * 
 * @see UWeaponFactory
 */
UCLASS()
class WEAPONSYSTEMPLUGIN_API UWeaponDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

						/* === C++ member functions === */
public:  
	UWeaponDataAsset() = default;
	virtual ~UWeaponDataAsset() = default		;
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;

	/** ============================= Getters ============================= */
	const bool GetDoesSupportAiming() const { return bDoesSupportAiming; }
	const bool GetHasProjectile() const { return  bHasProjectile; }
	const bool GetHasMesh() const { return bHasMesh; }
	const FGameplayTag& GetWeaponTag() const { return WeaponTag; }
	const TSubclassOf<AWeaponBase> GetWeaponClass() const { return WeaponClass; }
	const TObjectPtr<UStreamableRenderAsset> GetMesh() const { return Mesh; }
	const TSubclassOf<AProjectileBase> GetProjectileClass() const { return ProjectileClass; }
	const TSubclassOf<ATargetIndicator> GetIndicatorClass() const { return IndicatorClass; }
	const float GetDamage() const { return Damage; }
	const float GetAttackRange() const { return AttackRange; }
	const TSubclassOf<UWspDamageType> GetDamageTypeClass() const { return DamageTypeClass; }
	/** ==================================================================== */

						
						/* === Unreal Engine UPROPERTY === */
protected:
	/**
	 * @brief Only for Editor. If true, then Mesh property will be shown.
	 * @see Mesh
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Default | DetailSettings"/*, meta = (InlineEditConditionToggle)*/)
	bool bHasMesh{true};

	/**
	 * @brief Only for Editor. If true, then ProjectileClass property will be shown.
	 * @see ProjectileClass
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Default | DetailSettings"/*, meta = (InlineEditConditionToggle)*/)
	bool bHasProjectile{false};

	/**
	 * @brief Only for Editor. If true, then IndicatorClass and ProjectileClass properties will be shown.
	 * @see IndicatorClass ProjectileClass
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Default | DetailSettings"/*, meta = (InlineEditConditionToggle)*/)
	bool bDoesSupportAiming{false};

	/**
	 * @brief A unique tag that identifies the weapon type. Throwable items are also considered weapons.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon", meta = (DisplayName = "WeaponTag (required for all)") )
	FGameplayTag WeaponTag{};

	/**
	 * @brief The class type of the weapon.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon", meta = (DisplayName = "WeaponClass (required for all)") )
	TSubclassOf<AWeaponBase> WeaponClass{};

	/**
	 * @brief The class type of the damage type.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon", meta = (DisplayName = "DamageTypeClass"))
	TSubclassOf<UWspDamageType> DamageTypeClass{};

	/**
	 * @brief Can be Static or Skeletal mesh. The WeaponBase will create a MeshComponent of the appropriate type at runtime.
	 * @see WeaponBase::MeshComponent
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon", meta = (DisplayName = "Mesh (required)",
		EditCondition = "bHasMesh", EditConditionHides))
	TObjectPtr<UStreamableRenderAsset> Mesh{};

	/**
	 * @brief The class type of the projectile. Required for weapons that shoot projectiles or support aiming.
	 * @see AProjectileBase
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon", meta = (
		DisplayName = "ProjectileClass (required for grimoire or any weapon that supports aiming)"
		, EditCondition = "bHasProjectile || bDoesSupportAiming", EditConditionHides))
	TSubclassOf<AProjectileBase> ProjectileClass{};
		
	/**
	 * @brief The class type of the indicator. Required for weapons that support aiming.
	 * @see ATargetIndicator
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon", meta = (DisplayName = "IndicatorClass (required)",
		EditCondition = "bDoesSupportAiming", EditConditionHides))
	TSubclassOf<ATargetIndicator> IndicatorClass;

	/**
	 * @brief The amount of damage this weapon deals. Required for all weapons.
	 * Current implementation dosen't use this property.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon", meta = (DisplayName = "Damage (required for all)") )
	float Damage{10.f};

	/**
	 * @brief The maximum range at which this weapon can effectively attack. Required for all weapons.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon", meta = (DisplayName = "AttackRange (required for all)"))
	float AttackRange{300.f};
		
};


/*Weapon. Required for bows only*/
//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Not ready. Don't use.") 
//TSubclassOf<UAnimInstance> WeaponAnimBPClass{};

//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
//float AttackSpeed;

//
//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon", meta = (DisplayName = "ExplosionClass (required)",
//	EditCondition = "bDoesSupportAiming", EditConditionHides))
//TSubclassOf<AAreaExplosion> ExplosionClass;
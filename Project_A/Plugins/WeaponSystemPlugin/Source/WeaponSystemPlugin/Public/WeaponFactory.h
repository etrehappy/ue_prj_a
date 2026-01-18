/*****************************************************************//**
 * \file   WeaponFactory.h
 * \brief  This class is responsible for spawning weapon instances based on gameplay tags.
 * This mechanic was chosen to practise using GameplayTags in Unreal Engine and Data-Driven Development.
 * 
 * \date   January 2026
 *********************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "WeaponBase.h"

#include "WeaponFactory.generated.h"

/**
 * @class UWeaponFactory
 * @brief Factory spawns weapon instances based on gameplay tags. Specific weapon properties are defined in UWeaponDataAsset.
 * Enables weapons to be spawned independently of their class.
 * 
 * @see UWeaponDataAsset
 * @see AWeaponBase
 */
UCLASS()
class WEAPONSYSTEMPLUGIN_API UWeaponFactory : public UObject
{
	GENERATED_BODY()

					/* === C++ member functions === */
public:
	UWeaponFactory() = default;
	virtual ~UWeaponFactory() = default;

	/**
	 * @brief Spawns a weapon instance in the game world based on the provided gameplay tag.
	 * @details Current calling sequence:
	 * - After input, the Character calls UWeaponComponent::EquipWeaponByTag
	 * - UWeaponComponent::EquipWeaponByTag calls UWeaponFactory::SpawnWeaponByTag 
	 * 
	 * @param WeaponTag The gameplay tag identifies the type of weapon that is to be spawned.	 
	 * @return A pointer to the spawned weapon instance, or nullptr if spawning failed.
	 * @see UWeaponComponent::Server_EquipWeaponByTag_Implementation
	 */
	static AWeaponBase* SpawnWeaponByTag(UWorld* World, const FGameplayTag WeaponTag, const FActorSpawnParameters& SpawnParams, const FTransform& SpawnTransform);

private:
	/**
	 * @brief Loads all weapon data assets into the cache for quick access during weapon spawning.
	 * 
	 */
	static void LoadAllWeaponAssets();


					/* === C++ member variables === */
private:
	/**
	 * @brief Cached list of all weapon data assets for quick access during weapon spawning.
	 * @see LoadAllWeaponAssets
	 */
	static TArray<TSoftObjectPtr<UWeaponDataAsset>> CachedWeaponAssets;
};



#pragma once

#include "CoreMinimal.h"

#include "WeaponBase.h"

#include "WeaponFactory.generated.h"

UCLASS()
class WEAPONSYSTEMPLUGIN_API UWeaponFactory : public UObject
{
	GENERATED_BODY()

public:
	UWeaponFactory() = default;
    virtual ~UWeaponFactory() = default;

    static AWeaponBase* SpawnWeaponByTag(UWorld* World, const FGameplayTag WeaponTag, const FActorSpawnParameters& SpawnParams, const FTransform& SpawnTransform);

private:
    static void LoadAllWeaponAssets();

    static TArray<TSoftObjectPtr<UWeaponDataAsset>> CachedWeaponAssets;
};

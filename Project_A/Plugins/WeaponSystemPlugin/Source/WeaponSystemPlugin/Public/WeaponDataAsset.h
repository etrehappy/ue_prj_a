

#pragma once

#include "CoreMinimal.h"
#include "WeaponBase.h"

#include "WeaponDataAsset.generated.h"

/**
 * 
 */

UCLASS()
class WEAPONSYSTEMPLUGIN_API UWeaponDataAsset : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    UWeaponDataAsset() = default;
    virtual ~UWeaponDataAsset() = default;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
    FGameplayTag WeaponTag{};

    UPROPERTY(EditDefaultsOnly)
    UStaticMesh* StaticMesh{};

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
    TSubclassOf<AWeaponBase> WeaponClass{};

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
    float Damage{10.f};

    //UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
    //float AttackSpeed;

    virtual FPrimaryAssetId GetPrimaryAssetId() const override;
};

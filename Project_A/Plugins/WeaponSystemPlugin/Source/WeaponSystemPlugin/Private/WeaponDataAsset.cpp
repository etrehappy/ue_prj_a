#include "WeaponDataAsset.h"

FPrimaryAssetId UWeaponDataAsset::GetPrimaryAssetId() const
{
    return FPrimaryAssetId("Weapon", GetFName());
}

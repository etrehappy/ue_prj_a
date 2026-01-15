#include "WeaponDataAsset.h"
#include "WeaponPluginLog.h"


FPrimaryAssetId UWeaponDataAsset::GetPrimaryAssetId() const
{
    return FPrimaryAssetId("Weapon", GetFName());
}

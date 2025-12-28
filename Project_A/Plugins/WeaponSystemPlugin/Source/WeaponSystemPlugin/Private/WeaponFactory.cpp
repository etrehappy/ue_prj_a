
#include "WeaponFactory.h"
#include "Engine/AssetManager.h"
#include "WeaponDataAsset.h"
#include "Engine/StreamableManager.h"
#include "WeaponPluginLog.h"

TArray<TSoftObjectPtr<UWeaponDataAsset>> UWeaponFactory::CachedWeaponAssets;

AWeaponBase* UWeaponFactory::SpawnWeaponByTag(UWorld* World, const FGameplayTag WeaponTag, const FActorSpawnParameters& SpawnParams, const FTransform& SpawnTransform)
{
    if (!World)
    {
        UE_LOGFMT(LogWeaponPlugin, Warning, "{0} — Invalid World", FString(__FUNCTION__));
        return nullptr;
    }
        
    if (World->GetNetMode() == NM_Client)
    {
        UE_LOGFMT(LogWeaponPlugin, Warning, "{0} — Spawn attempted on client. Abort. Tag {1}", FString(__FUNCTION__), *WeaponTag.ToString());
        return nullptr;
    }

    LoadAllWeaponAssets();

   // UE_LOGFMT(LogWeaponPlugin, Warning, "{0} — Weapon not found for tag {1}", FString(__FUNCTION__), CachedWeaponAssets.Num());

    //for (UWeaponDataAsset* Asset : CachedWeaponAssets)
    for (const TSoftObjectPtr<UWeaponDataAsset>& SoftPtr : CachedWeaponAssets)
    {
        UWeaponDataAsset* Asset = SoftPtr.LoadSynchronous();
        if (!Asset)
        {
            UE_LOGFMT(LogWeaponPlugin, Warning, "{0} — Cached soft asset failed to load", FString(__FUNCTION__));
            continue;
        }


        if (Asset->WeaponTag == WeaponTag)
        {
            AWeaponBase* Weapon = World->SpawnActor<AWeaponBase>(Asset->WeaponClass, SpawnTransform, SpawnParams);

            if (!Weapon)
            {
                UE_LOGFMT(LogWeaponPlugin, Warning, "{0} — Weapon not Spawn. Tag {1}", FString(__FUNCTION__), *WeaponTag.ToString());                
                return nullptr;
            }

            Weapon->InitializeFromData(Asset);
            return Weapon;
        }        
    }

    UE_LOGFMT(LogWeaponPlugin, Warning, "{0} — Weapon not found for tag {1}", FString(__FUNCTION__), *WeaponTag.ToString());
    return nullptr;
}

void UWeaponFactory::LoadAllWeaponAssets()
{    
    if (!CachedWeaponAssets.IsEmpty() ) { return; }        

    TArray<FAssetData> AssetData{};
    UAssetManager::Get().GetPrimaryAssetDataList(FPrimaryAssetType("Weapon"), AssetData);

    for (auto& Data : AssetData)
    {        
        FSoftObjectPath SoftPath = Data.ToSoftObjectPath();
    /*    if (auto* Asset = Cast<UWeaponDataAsset>(Data.GetAsset() ))
        {
            CachedWeaponAssets.Add(Asset);
        }*/

        if (!SoftPath.IsValid())
        {
            continue;
        }

        TSoftObjectPtr<UWeaponDataAsset> SoftPtr(SoftPath);
        CachedWeaponAssets.Add(SoftPtr);

#if UE_BUILD_DEVELOPMENT
        UE_LOGFMT(LogWeaponPlugin, Log, "{0} — Cached soft asset path: {1}", FString(__FUNCTION__), *SoftPath.ToString());
#endif
    }


}

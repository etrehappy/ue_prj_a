// Copyright Epic Games, Inc. All Rights Reserved.

#include "WeaponSystemPlugin.h"
#include "GameplayTagsManager.h"
#include "Engine/AssetManagerSettings.h"
#include "WeaponPluginLog.h"

DEFINE_LOG_CATEGORY(LogWeaponPlugin);

#define LOCTEXT_NAMESPACE "FWeaponSystemPluginModule"

void FWeaponSystemPluginModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	UGameplayTagsManager::Get().AddTagIniSearchPath(FPaths::ProjectPluginsDir() / TEXT("WeaponSystemPlugin/Config/Tags"));
	AddPrimaryAssetType();

}

void FWeaponSystemPluginModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

void FWeaponSystemPluginModule::AddPrimaryAssetType()
{  
    UAssetManagerSettings* Settings = GetMutableDefault<UAssetManagerSettings>();

    FPrimaryAssetTypeInfo NewType;
    NewType.PrimaryAssetType = TEXT("Weapon");
    NewType.SetAssetBaseClass(TSoftClassPtr<UObject>(FSoftClassPath(TEXT("/Script/WeaponSystemPlugin.WeaponDataAsset"))));
    NewType.bHasBlueprintClasses = false;

    bool bAreSettingsExists = false;
    for (const auto& Type : Settings->PrimaryAssetTypesToScan)
    {
        if (Type.PrimaryAssetType == NewType.PrimaryAssetType)
        {
            bAreSettingsExists = true;
            break;
        }
    }

    if (!bAreSettingsExists)
    {
        Settings->PrimaryAssetTypesToScan.Add(NewType);
        Settings->SaveConfig();
    }
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FWeaponSystemPluginModule, WeaponSystemPlugin)

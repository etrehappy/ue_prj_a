#include "StatusEffect/StatusEffectSubsystem.h"
#include "StatusEffect/StatusEffectData.h"
#include "StatusEffect/StatusEffectTypes.h"
#include "Engine/AssetManager.h"

#include "ProjectALog.h"

void UStatusEffectSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	LoadEffectDefinitionsFromConfig();
	BuildEffectMap();
}

const FStatusEffectDef* UStatusEffectSubsystem::GetEffectByTag(const FGameplayTag& Tag) const
{
	if (const FStatusEffectDef* Found = EffectMap.Find(Tag))
	{
		return Found;
	}
	return nullptr;
}

void UStatusEffectSubsystem::LoadEffectDefinitionsFromConfig()
{
	for (const TSoftObjectPtr<UStatusEffectData>& DataSoft : StartupStatusEffectDefinitions)
	{
		if (DataSoft.IsNull())
		{
			continue;
		}

		UStatusEffectData* Data = DataSoft.LoadSynchronous();
		if (!Data)
		{
			UE_LOGFMT(LogProjectA, Warning, "{0} - Failed to load status effect data asset from config", *FString(__FUNCTION__));
			continue;
		}

		RegisterEffectDataAsset(Data);
	}
}

void UStatusEffectSubsystem::RegisterEffectDataAsset(UStatusEffectData* DataAsset)
{
	if (!DataAsset)
	{
		return;
	}

	for (const FStatusEffectDef& Spec : DataAsset->GetEffectSpecs())
	{
		if (!Spec.EffectTag.IsValid())
		{
			UE_LOGFMT(LogProjectA, Warning, "{0} - Invalid EffectTag in asset {1}", *FString(__FUNCTION__), DataAsset->GetName());
			continue;
		}

		if (!EffectMap.Contains(Spec.EffectTag))
		{
			EffectMap.Add(Spec.EffectTag, Spec);
		}
	}
}

void UStatusEffectSubsystem::BuildEffectMap()
{
	UAssetManager& Manager = UAssetManager::Get();

	TArray<FPrimaryAssetId> AssetIds;
	Manager.GetPrimaryAssetIdList(FPrimaryAssetType(TEXT("StatusEffect")), AssetIds);

	for (const FPrimaryAssetId& Id : AssetIds)
	{
		UObject* LoadedObj = Manager.GetPrimaryAssetObject(Id);

		if (!LoadedObj)
		{
			const FSoftObjectPath Path = Manager.GetPrimaryAssetPath(Id);
			LoadedObj = Path.TryLoad();
		}

		UStatusEffectData* Data = Cast<UStatusEffectData>(LoadedObj);
		if (!Data)
		{
			continue;
		}

		RegisterEffectDataAsset(Data);
	}

	UE_LOGFMT(LogProjectA, Log, "{0} - Loaded status effects: {1}", *FString(__FUNCTION__), EffectMap.Num());
}
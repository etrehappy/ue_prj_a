#include "StatusEffect/StatusEffectSubsystem.h"
#include "StatusEffect/StatusEffectData.h"
#include "StatusEffect/StatusEffectTypes.h"
#include "Engine/AssetManager.h"

#include "ProjectALog.h"

void UStatusEffectSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

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

void UStatusEffectSubsystem::BuildEffectMap()
{
    if (!EffectMap.IsEmpty())
    {
		UE_LOGFMT(LogProjectA, Warning, "{0} - EffectMap is not empty, skipping BuildEffectMap to avoid overwriting existing data", *FString(__FUNCTION__));
        return;
    }
    
    UAssetManager& Manager = UAssetManager::Get();

    TArray<FPrimaryAssetId> AssetIds;
    Manager.GetPrimaryAssetIdList(FPrimaryAssetType(TEXT("StatusEffect")), AssetIds);

    for (const FPrimaryAssetId& Id : AssetIds)
    {

        UObject* LoadedObj = Manager.GetPrimaryAssetObject(Id);

        if (!LoadedObj)
        {
            FSoftObjectPath Path = Manager.GetPrimaryAssetPath(Id);
            LoadedObj = Path.TryLoad();
        }

        if (!LoadedObj)
        {
            UE_LOGFMT(LogProjectA, Warning, "{0} - Could not load asset for {1}", *FString(__FUNCTION__), Id.ToString());
            continue;
		}

        UStatusEffectData* Data = Cast<UStatusEffectData>(LoadedObj);
        if(!Data)
        {
            UE_LOGFMT(LogProjectA, Warning, "{0} - Could not load UStatusEffectData for asset {1}", *FString(__FUNCTION__), Id.ToString());
			continue;
        }

     
        for (const FStatusEffectDef& Spec : Data->GetEffectSpecs())
        {
            if (!Spec.EffectTag.IsValid())
            {
                UE_LOGFMT(LogProjectA, Warning, "{0} - Invalid EffectTag in UStatusEffectData {1}", *FString(__FUNCTION__), Id.ToString());
				continue;
            }

            if (!EffectMap.Contains(Spec.EffectTag))
            {
                EffectMap.Add(Spec.EffectTag, Spec);
				//UE_LOGFMT(LogProjectA, Log, "{0} - Added effect with tag {1} from asset {2}", *FString(__FUNCTION__), Spec.EffectTag.ToString(), Id.ToString());
            }
            
        }
       
    }
}


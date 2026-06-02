/*****************************************************************//**
 * \file   StatusEffectSubsystem.h
 * \brief  It was created to load an effects data in memory.
 * 
 * \date   April 2026
 *********************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameplayTagContainer.h"
#include "StatusEffect/StatusEffectTypes.h"

#include "StatusEffectSubsystem.generated.h"

class UStatusEffectData;

/**
 * @class UStatusEffectSubsystem
 * @brief Subsystem responsible for managing status effects in the game.
 */
UCLASS(Config = Game)
class COMMON_API UStatusEffectSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    const FStatusEffectDef* GetEffectByTag(const FGameplayTag& Tag) const;

private:
    /**
     * @brief Builds the map of status effects for quick lookup by gameplay tag.
     */
    void BuildEffectMap();

    void LoadEffectDefinitionsFromConfig();
    void RegisterEffectDataAsset(UStatusEffectData* DataAsset);

    UPROPERTY(Config, EditDefaultsOnly, Category = "StatusEffects")
    TArray<TSoftObjectPtr<UStatusEffectData>> StartupStatusEffectDefinitions{};

    
    /**
	 * @brief There is a effect's primary data asset. The subsystem loads all assets of this type and builds a map for quick access to effect definitions by their gameplay tags.
     * 
     * @see FStatusEffectDef
     * @see UStatusEffectData
     */
    UPROPERTY()
    TMap<FGameplayTag, FStatusEffectDef> EffectMap{};
};

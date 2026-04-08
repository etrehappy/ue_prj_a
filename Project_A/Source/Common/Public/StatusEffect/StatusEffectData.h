/*****************************************************************//**
 * \file   StatusEffectData.h
 * \brief  Simple way to combine all status effect specifications in one place.
 * 
 * \date   April 2026
 *********************************************************************/

#pragma once
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Engine/AssetManager.h"
#include "StatusEffectTypes.h"

#include "StatusEffectData.generated.h"



/**
 * @class UStatusEffectData
 * @brief Data asset that contains a list of status effect specifications.
 * 
 * @see UStatusEffectSubsystem
 * @see FStatusEffectDef
 */
UCLASS(BlueprintType)
class COMMON_API UStatusEffectData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UStatusEffectData() = default;
	virtual ~UStatusEffectData() = default;
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;

	/**
	 * @see UStatusEffectSubsystem
	 */
	const TArray<FStatusEffectDef>& GetEffectSpecs() const { return EffectSpecs; }

protected:
		
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "StatusEffects")
	TArray<FStatusEffectDef> EffectSpecs{};
};
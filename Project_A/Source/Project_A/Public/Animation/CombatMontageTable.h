

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "CombatMontageTable.generated.h"

USTRUCT(BlueprintType)
struct FCombatMontageRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTagContainer WeaponTags{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag AbilityTag{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> Montage{};
};




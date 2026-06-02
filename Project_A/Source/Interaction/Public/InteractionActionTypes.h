#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"

#include "InteractionActionTypes.generated.h"

class UInteractionActionDefinition;

USTRUCT(BlueprintType)
struct FInteractionActionType
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsEnabled{true};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<const UInteractionActionDefinition> Definition{};
};


UCLASS(BlueprintType)
class INTERACTION_API UInteractionActionDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag ActionTag{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText ActionName{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bEnabledByDefault{true};
};
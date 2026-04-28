

#pragma once
#include "CoreMinimal.h"

#include "GameplayTagContainer.h"
#include "Engine/DataTable.h"
#include "NpcTypes.h"

#include "EnemyRow.generated.h"

class AEnemyBase;

/**
 * 
 */
USTRUCT(BlueprintType)
struct FEnemyRow : public FTableRowBase
{
    GENERATED_BODY();

public:
    FEnemyRow() = default;
    virtual ~FEnemyRow() = default;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FGameplayTag EnemyTag;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<class AEnemyBase> EnemyClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float MaxHealth{100};

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Damage{10};

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float MoveSpeed{300};

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bIsFriendly{false};

};

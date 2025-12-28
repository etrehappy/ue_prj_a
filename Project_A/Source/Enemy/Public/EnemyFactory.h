

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

#include "EnemyFactory.generated.h"

class UDataTable;
class AEnemyBase;

UCLASS()
class ENEMY_API UEnemyFactory : public UObject
{
	GENERATED_BODY()
	
public:
	UEnemyFactory() = default;
	virtual ~UEnemyFactory() = default;   


public:
    UFUNCTION(BlueprintCallable, Category = "Enemy Factory", meta = (WorldContext = "World"))
    AEnemyBase* SpawnEnemyByTag(UWorld* World, UDataTable* EnemyDataTable, FGameplayTag EnemyTag, const FVector& Location, const FRotator& Rotation);

};

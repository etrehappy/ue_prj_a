

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "NpcBase.h"
#include "EnemyRow.h"
#include "HealthComponent.h"
#include "Engine/EngineTypes.h"

#include "EnemyBase.generated.h"

class UWeaponComponent;
class AWeaponBase;


UCLASS()
class ENEMY_API AEnemyBase : public /*ABasePawn*/ ANpcBase
{
	GENERATED_BODY()

						/* === C++ member functions === */
public:	
	AEnemyBase();
	void InitializeFromTableData(const FEnemyRow& Data);

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	//virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
		
protected:
	
};

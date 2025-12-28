

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Actor.h"
#include "BasePawn.h"
#include "EnemyRow.h"

#include "EnemyBase.generated.h"

UCLASS()
class ENEMY_API AEnemyBase : public ABasePawn
{
	GENERATED_BODY()
	
public:	
	AEnemyBase();
	virtual void Tick(float DeltaTime) override;
	void InitializeFromTableData(const FEnemyRow& Data);
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void BeginPlay() override;


private:

	UPROPERTY(EditDefaultsOnly, Replicated, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float MaxHealth{100};

	UPROPERTY(EditDefaultsOnly, Replicated, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float Damage{10};

	UPROPERTY(EditDefaultsOnly, Replicated, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float MoveSpeed{300};
	
	
};

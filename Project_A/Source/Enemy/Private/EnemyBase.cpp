#include "EnemyBase.h"
#include "EnemyRow.h"
#include "Net/UnrealNetwork.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"

#include "ProjectALog.h"


// Sets default values
AEnemyBase::AEnemyBase()
{
    bReplicates = true;
	//PrimaryActorTick.bCanEverTick = true;
}

void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void AEnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	
}




void AEnemyBase::InitializeFromTableData(const FEnemyRow& Data)
{
	const ENpcFaction Faction = Data.bIsFriendly ? ENpcFaction::Friendly : ENpcFaction::Enemy;
	SetNpcCoreData(Data.MaxHealth, Data.Damage, Data.MoveSpeed, Faction);
}



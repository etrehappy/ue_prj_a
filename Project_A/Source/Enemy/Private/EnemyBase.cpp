


#include "EnemyBase.h"
#include "EnemyRow.h"
#include "Net/UnrealNetwork.h"
#include "ProjectALog.h"


// Sets default values
AEnemyBase::AEnemyBase()
{
	PrimaryActorTick.bCanEverTick = false;
    bReplicates = true;
}

void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEnemyBase::InitializeFromTableData(const FEnemyRow& Data)
{
    if (!HasAuthority())
    {
        UE_LOGFMT(LogProjectA, Warning, "{0} — InitializeFromTableData should be called only on server", FString(__FUNCTION__));
        return;
    }

    MaxHealth = Data.MaxHealth;
    Damage = Data.Damage;
    MoveSpeed = Data.MoveSpeed;
}

void AEnemyBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AEnemyBase, MaxHealth);
    DOREPLIFETIME(AEnemyBase, Damage);
    DOREPLIFETIME(AEnemyBase, MoveSpeed);
}



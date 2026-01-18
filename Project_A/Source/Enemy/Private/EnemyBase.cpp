


#include "EnemyBase.h"
#include "EnemyRow.h"
#include "Net/UnrealNetwork.h"
#include "ProjectALog.h"


// Sets default values
AEnemyBase::AEnemyBase()
{
	PrimaryActorTick.bCanEverTick = false;
    bReplicates = true;

    HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
    HealthComponent->SetComponentTickEnabled(false);
}

void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();
    HealthComponent->OnDeath.AddDynamic(this, &AEnemyBase::OnDead);
    HealthComponent->OnIncreaseHealth.AddDynamic(this, &AEnemyBase::OnIncreaseHealth);
    HealthComponent->OnDecreaseHealth.AddDynamic(this, &AEnemyBase::OnDecreaseHealth);
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
        UE_LOGFMT(LogProjectA, Warning, "{0} - InitializeFromTableData should be called only on server", FString(__FUNCTION__));
        return;
    }

    HealthComponent->SetMaxHealth(Data.MaxHealth);
    Damage = Data.Damage;
    MoveSpeed = Data.MoveSpeed;
}

void AEnemyBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
        
    DOREPLIFETIME(AEnemyBase, Damage);
    DOREPLIFETIME(AEnemyBase, MoveSpeed);
}


void AEnemyBase::OnDead()
{
	UE_LOGFMT(LogProjectA, Log, "{0} - called ", FString(__FUNCTION__));
	Multicast_PlayDeathFX();

	if(IsRunningDedicatedServer())
	{
		if (DeathLifeSpan <= 0.f)
		{
			Destroy();
		}
		else
		{
			GetWorldTimerManager().SetTimer(DeathTimerHandle, this, &AEnemyBase::DestroyEnemy, DeathLifeSpan, false);
		}
	}
}

void AEnemyBase::DestroyEnemy()
{
	Destroy();	
}

void AEnemyBase::OnIncreaseHealth(float HealAmount)
{
	UE_LOGFMT(LogProjectA, Log, "{0} - called ", FString(__FUNCTION__));
	Multicast_PlayHealFX(HealAmount);
}

void AEnemyBase::OnDecreaseHealth(float DamageAmount)
{
	UE_LOGFMT(LogProjectA, Log, "{0} - called ", FString(__FUNCTION__));
	Multicast_PlayDamageFX(DamageAmount);
}

void AEnemyBase::Multicast_PlayHealFX_Implementation(float HealAmount)
{
	UE_LOGFMT(LogProjectA, Log, "{0} - called ", FString(__FUNCTION__));
	if (IsRunningDedicatedServer())
	{
		UE_LOGFMT(LogProjectA, Log, "{0} - called on a dedicated server, skipping ", FString(__FUNCTION__));
		return;
	}

	OnPlayHealFX(HealAmount);
}

void AEnemyBase::Multicast_PlayDamageFX_Implementation(float DamageAmount)
{
	UE_LOGFMT(LogProjectA, Log, "{0} - called ", FString(__FUNCTION__));
	if (IsRunningDedicatedServer())
	{
		UE_LOGFMT(LogProjectA, Log, "{0} - called on a dedicated server, skipping ", FString(__FUNCTION__));
		return;
	}

	OnPlayDamageFX(DamageAmount);
}

void AEnemyBase::Multicast_PlayDeathFX_Implementation()
{
	UE_LOGFMT(LogProjectA, Log, "{0} - called ", FString(__FUNCTION__));
	if (IsRunningDedicatedServer())
	{
		UE_LOGFMT(LogProjectA, Log, "{0} - called on a dedicated server, skipping ", FString(__FUNCTION__));
		return;
	}

	OnPlayDeathFX();
}

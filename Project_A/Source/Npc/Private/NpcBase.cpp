#include "NpcBase.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "AIController.h"
#include "BrainComponent.h"
#include "StatusEffect/StatusEffectsComponent.h"
#include "NpcBattleComponent.h"

#include "ProjectALog.h"

ANpcBase::ANpcBase()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	SetReplicateMovement(true);

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
	HealthComponent->SetComponentTickEnabled(false);

	StatusEffectComponent = CreateDefaultSubobject<UStatusEffectComponent>(TEXT("StatusEffectComponent"));
	StatusEffectComponent->SetComponentTickEnabled(false);

	NpcBattleComponent = CreateDefaultSubobject<UNpcBattleComponent>(TEXT("NpcBattleComponent"));
	//NpcBattleComponent->SetComponentTickEnabled(true);
}

void ANpcBase::DestroyNpc()
{
	Destroy();
}

void ANpcBase::BeginPlay()
{
	Super::BeginPlay();

	if (HealthComponent)
	{
		HealthComponent->OnDeath.AddDynamic(this, &ANpcBase::OnDead);
		HealthComponent->OnIncreaseHealth.AddDynamic(this, &ANpcBase::OnIncreaseHealth);
		HealthComponent->OnDecreaseHealth.AddDynamic(this, &ANpcBase::OnDecreaseHealth);
	}

	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = MoveSpeed;
	}
}

void ANpcBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ANpcBase, MoveSpeed);
	DOREPLIFETIME(ANpcBase, NpcFaction);
	DOREPLIFETIME(ANpcBase, bIsDead);
	DOREPLIFETIME(ANpcBase, CurrentTarget);
	DOREPLIFETIME(ANpcBase, NpcBattleComponent);
	DOREPLIFETIME(ANpcBase, bIsAttacking);
}

void ANpcBase::SetNpcCoreData(float InMaxHealth, float InDamage, float InMoveSpeed, ENpcFaction InFaction)
{
	if (!HasAuthority())
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - should be called only on authority", FString(__FUNCTION__));
		return;
	}

	if (HealthComponent && InMaxHealth > 0.f)
	{
		HealthComponent->SetMaxHealth(InMaxHealth);
	}

	if (NpcBattleComponent)
	{
		NpcBattleComponent->SetDamageAmount(InDamage);
	}

	MoveSpeed = InMoveSpeed;
	NpcFaction = InFaction;

	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = MoveSpeed;
	}
}

void ANpcBase::SetCurrentTarget(AActor* NewTarget)
{
	if (!IsRunningDedicatedServer())
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - should be called only on the server. Actor: {1}", FString(__FUNCTION__), *GetName());
		return;
	}

	CurrentTarget = NewTarget;
	OnCurrentTargetChanged(CurrentTarget);
}

void ANpcBase::SetIsAttacking(bool bNewIsAttacking)
{
	if (!IsRunningDedicatedServer())
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - SetIsAttacking should only be called on the server. Actor: {1}", FString(__FUNCTION__), *GetName());
		return;
	}

	bIsAttacking = bNewIsAttacking;
}


void ANpcBase::OnDead()
{
	if (!IsRunningDedicatedServer()) { return; }

	bIsDead = true;
	Multicast_PlayDeathFX();
		
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->DisableMovement();
		GetCharacterMovement()->bOrientRotationToMovement = false;
	}

	if (GetCapsuleComponent())
	{
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
		
	if (AAIController* AICon = Cast<AAIController>(GetController()))
	{
		AICon->ClearFocus(EAIFocusPriority::Gameplay);
		AICon->StopMovement();
		if (UBrainComponent* Brain = AICon->BrainComponent)
		{
			Brain->StopLogic(TEXT("Dead"));
		}
	}
		
	bUseControllerRotationYaw = false;
	if (AController* C = GetController())
	{
		C->SetControlRotation(GetActorRotation());
	}

	if (DeathLifeSpan <= 0.f)
	{
		DestroyNpc();
	}
	else
	{
		GetWorldTimerManager().SetTimer(DeathTimerHandle, this, &ANpcBase::DestroyNpc, DeathLifeSpan, false);
	}
}

void ANpcBase::OnIncreaseHealth(float HealAmount)
{
	Multicast_PlayHealFX(HealAmount);
}

void ANpcBase::OnDecreaseHealth(float DamageAmount)
{
	Multicast_PlayDamageFX(DamageAmount);
}

void ANpcBase::OnRep_CurrentTarget()
{
	OnCurrentTargetChanged(CurrentTarget);
}

void ANpcBase::Multicast_PlayHealFX_Implementation(float HealAmount)
{
	if (IsRunningDedicatedServer()) { return; }

	OnPlayHealFX(HealAmount);
}

void ANpcBase::Multicast_PlayDamageFX_Implementation(float DamageAmount)
{
	if (IsRunningDedicatedServer()) { return; }

	OnPlayDamageFX(DamageAmount);
}

void ANpcBase::Multicast_PlayDeathFX_Implementation()
{
	if (IsRunningDedicatedServer()) { return; }

	OnPlayDeathFX();
}

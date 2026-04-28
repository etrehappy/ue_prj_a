#include "NpcBattleComponent.h"

#include "NpcAttackLogicBase.h"
#include "GameFramework/Actor.h"
#include "Net/UnrealNetwork.h"
#include "NpcBase.h"

#include "ProjectALog.h"

UNpcBattleComponent::UNpcBattleComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
}

void UNpcBattleComponent::BeginPlay()
{
	Super::BeginPlay();

	SetComponentTickEnabled(false);

	if (AttackLogic)
	{
		AttackLogicInstance = NewObject<UNpcAttackLogicBase>(this, AttackLogic);
		if (AttackLogicInstance)
		{
			AttackLogicInstance->Initialize(this);
		}
	}
	else
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - AttackLogic is not configured. Actor: {1}", FString(__FUNCTION__), *GetName());
	}
}

//It works only when necessary. It is controlled via SetComponentTickEnabled(true)
void UNpcBattleComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
		
	if (AttackLogicInstance && AttackLogicInstance->WantsTick())
	{
		AttackLogicInstance->TickDamageWindow(DeltaTime);
	}	
}

void UNpcBattleComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UNpcBattleComponent, bIsAttacking);
	DOREPLIFETIME(UNpcBattleComponent, Damage);
	//DOREPLIFETIME(UNpcBattleComponent, DamageTypeClass);
}

bool UNpcBattleComponent::IsAttacking() const
{
	return bIsAttacking;
}

void UNpcBattleComponent::StartAttack()
{
	//1. Validations
	if (!IsRunningDedicatedServer())
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - StartAttack should only be called on the server. Actor: {1}", FString(__FUNCTION__), *GetName());
		return;
	}

	ANpcBase* NpcOwner = Cast<ANpcBase>(GetOwner());
	if (!NpcOwner)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Owner is not an ANpcBase", FString(__FUNCTION__));
		return;
	}

	if (!AttackLogicInstance)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - AttackLogic is null, cannot attack. Actor: {1}", FString(__FUNCTION__), *GetName());
		return;
	}

	const float CurrentTime = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.f;
	if (CurrentTime < NextAttackAllowedTime)
	{
		return;
	}

	AActor* Target = NpcOwner->GetCurrentTarget();
	if (!IsTargetInAttackRange(Target))
	{
		UE_LOGFMT(LogProjectA, Log, "{0} - target is not in range, cannot start attack. Actor: {1}", FString(__FUNCTION__), *GetName());
		return;
	}

	if (bIsAttacking)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - already attacking, cannot start another attack. Actor: {1}", FString(__FUNCTION__), *GetName());
		return;
	}

	if (!IsValid(AttackLogicInstance))
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - AttackLogic::CanStartAttack failed. Actor: {1}", FString(__FUNCTION__), *GetName());
		return;
	}

	//2. Start attack
	SetIsAttacking(true);
	AttackLogicInstance->OnAttackStarted();
	Multicast_PlayAttack();

	//3. Setup auto stop timer
	HandleAttackAutoStop(NpcOwner, CurrentTime);
}

void UNpcBattleComponent::SetIsAttacking(bool bNewIsAttacking)
{
	if (!IsRunningDedicatedServer())
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - SetIsAttacking should only be called on the server. Actor: {1}", FString(__FUNCTION__), *GetName());
		return;
	}
	bIsAttacking = bNewIsAttacking;
	ANpcBase* NpcOwner = Cast<ANpcBase>(GetOwner());
	if (NpcOwner)
	{
		NpcOwner->SetIsAttacking(bNewIsAttacking);
	}
}

void UNpcBattleComponent::HandleAttackAutoStop(ANpcBase* NpcOwner, const float CurrentTime)
{
	FString DebugFunctionName = FString(__FUNCTION__);
	NpcOwner->GetWorldTimerManager().ClearTimer(AttackAutoStopTimerHandle);
	NpcOwner->GetWorldTimerManager().SetTimer(
		AttackAutoStopTimerHandle,
		FTimerDelegate::CreateLambda([this, DebugFunctionName]()
			{
				StopAttack(DebugFunctionName);
			}),
		AttackMaxDuration,
		false);

	NextAttackAllowedTime = CurrentTime + AttackCooldown;
}

bool UNpcBattleComponent::IsTargetInAttackRange(const AActor* Target) const
{
	if (!Target)
	{
		return false;
	}

	const float DistanceSquared = FVector::DistSquared(GetOwner()->GetActorLocation(), Target->GetActorLocation());
	return DistanceSquared <= FMath::Square(AttackRange);
}

void UNpcBattleComponent::SetDamageAmount(float NewDamage)
{
	if (!IsRunningDedicatedServer())
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - SetDamageAmount should only be called on the server. Actor: {1}", FString(__FUNCTION__), *GetName());
		return;
	}

	if(NewDamage < 0.f)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - NewDamage cannot be negative. Actor: {1}, NewDamage: {2}", FString(__FUNCTION__), *GetName(), NewDamage);
		return;
	}

	Damage = NewDamage;
	return;
}

void UNpcBattleComponent::StopAttack(FString Caller)
{
	if (!IsRunningDedicatedServer())
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - StopAttack should only be called on the server. Actor: {1}, Caller: {2}", FString(__FUNCTION__), *GetName(), *Caller);
		return;
	}

	if (!bIsAttacking)
	{
		return;
	}

	ANpcBase* NpcOwner = Cast<ANpcBase>(GetOwner());
	if (!NpcOwner)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Owner is not an ANpcBase", FString(__FUNCTION__));
		return;
	}

	SetIsAttacking(false);
	bDamageWindowActive = false;

	if (AttackLogicInstance)
	{
		AttackLogicInstance->OnAttackStopped();
		AttackLogicInstance->OnDamageWindowEnded();
	}

	NpcOwner->GetWorldTimerManager().ClearTimer(AttackAutoStopTimerHandle);
}

void UNpcBattleComponent::BeginDamageWindow()
{
	if (!IsRunningDedicatedServer())
	{
		return;
	}

	if (!bIsAttacking)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Cannot begin damage window because melee attack is not active. Actor: {1}", FString(__FUNCTION__), *GetName());
		return;
	}

	if (!AttackLogicInstance)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - AttackLogic is null. Actor: {1}", FString(__FUNCTION__), *GetName());
		return;
	}

	if (!AttackLogicInstance->OnDamageWindowStarted())
	{
		return;
	}

	bDamageWindowActive = true;
}

void UNpcBattleComponent::EndDamageWindow()
{
	if (!IsRunningDedicatedServer())
	{
		return;
	}

	bDamageWindowActive = false;

	if (AttackLogicInstance)
	{
		AttackLogicInstance->OnDamageWindowEnded();
	}

	StopAttack(FString(__FUNCTION__));
}

void UNpcBattleComponent::Multicast_PlayAttack_Implementation()
{
	ANpcBase* NpcOwner = Cast<ANpcBase>(GetOwner());
	if (NpcOwner)
	{
		NpcOwner->OnPlayAttack();
	}
}

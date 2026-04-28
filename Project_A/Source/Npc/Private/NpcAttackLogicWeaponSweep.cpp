#include "NpcAttackLogicWeaponSweep.h"

#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "NpcBase.h"
#include "NpcBattleComponent.h"
#include "ProjectALog.h"

void UNpcAttackLogicWeaponSweep::Initialize(UNpcBattleComponent* InBattleComponent)
{
	Super::Initialize(InBattleComponent);
	InitialiseWeaponMeshComponent();
}

bool UNpcAttackLogicWeaponSweep::CanStartAttack() const
{
	return WeaponMeshComponent != nullptr;
}

void UNpcAttackLogicWeaponSweep::OnAttackStarted()
{
	HitActorsThisAttack.Reset();

	if (!WeaponMeshComponent)
	{
		InitialiseWeaponMeshComponent();
	}

	BattleComponent->SetComponentTickEnabled(true);
}

void UNpcAttackLogicWeaponSweep::OnAttackStopped()
{
	HitActorsThisAttack.Reset();
	BattleComponent->SetComponentTickEnabled(false);
}

bool UNpcAttackLogicWeaponSweep::OnDamageWindowStarted()
{
	if (!BattleComponent)
	{
		return false;
	}

	if (!WeaponMeshComponent)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - WeaponMeshComponent is not set. Actor: {1}", FString(__FUNCTION__), *BattleComponent->GetName());
		return false;
	}

	if (!WeaponMeshComponent->DoesSocketExist(TipSocketName) || !WeaponMeshComponent->DoesSocketExist(BaseSocketName))
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Missing Tip/Base sockets on WeaponMeshComponent", FString(__FUNCTION__));
		return false;
	}

	LastTipPosition = WeaponMeshComponent->GetSocketLocation(TipSocketName);
	LastBasePosition = WeaponMeshComponent->GetSocketLocation(BaseSocketName);
	return true;
}

void UNpcAttackLogicWeaponSweep::TickDamageWindow(float DeltaTime)
{
	if (!IsRunningDedicatedServer())
	{
		return;
	}

	if (!BattleComponent)
	{
		return;
	}

	ANpcBase* NpcOwner = Cast<ANpcBase>(BattleComponent->GetOwner());
	if (!NpcOwner)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Owner is not an ANpcBase", FString(__FUNCTION__));
		return;
	}

	if (BattleComponent->IsAttacking())
	{
		AActor* Target = NpcOwner->GetCurrentTarget();
		if (!Target || !BattleComponent->IsTargetInAttackRange(Target))
		{
			BattleComponent->StopAttack(FString(__FUNCTION__));
			return;
		}
	}

	if (!WeaponMeshComponent)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - WeaponMeshComponent is not set. Actor: {1}", FString(__FUNCTION__), *BattleComponent->GetName());
		return;
	}

	const FVector TipNow = WeaponMeshComponent->GetSocketLocation(TipSocketName);
	const FVector BaseNow = WeaponMeshComponent->GetSocketLocation(BaseSocketName);

	FCollisionQueryParams Params{};
	Params.AddIgnoredActor(NpcOwner);

	TArray<FHitResult> Hits{};
	ApplyHitsFromSweep(Hits, LastTipPosition, TipNow, Params, NpcOwner);
	ApplyHitsFromSweep(Hits, LastBasePosition, BaseNow, Params, NpcOwner);
	ApplyHitsFromSweep(Hits, BaseNow, TipNow, Params, NpcOwner);

	LastTipPosition = TipNow;
	LastBasePosition = BaseNow;
}

bool UNpcAttackLogicWeaponSweep::WantsTick() const
{
	return true;
}

void UNpcAttackLogicWeaponSweep::InitialiseWeaponMeshComponent()
{
	if (!BattleComponent)
	{
		return;
	}

	AActor* Owner = BattleComponent->GetOwner();
	if (!Owner)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Owner is null", FString(__FUNCTION__));
		return;
	}

	ACharacter* CharacterOwner = Cast<ACharacter>(Owner);
	if (!CharacterOwner)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Owner is not a character", FString(__FUNCTION__));
		return;
	}

	TArray<USkeletalMeshComponent*> SkeletalMeshComponents{};
	CharacterOwner->GetComponents<USkeletalMeshComponent>(SkeletalMeshComponents);

	for (USkeletalMeshComponent* MeshComponent : SkeletalMeshComponents)
	{
		if (MeshComponent && MeshComponent->ComponentHasTag(WeaponMeshComponentTag))
		{
			WeaponMeshComponent = MeshComponent;
			UE_LOGFMT(LogProjectA, Log, "{0} - Found WeaponMeshComponent on self: {1}", FString(__FUNCTION__), *WeaponMeshComponent->GetName());
			break;
		}
	}

	if (!WeaponMeshComponent)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - WeaponMeshComponent was not found by tag '{1}'", FString(__FUNCTION__), WeaponMeshComponentTag.ToString());
	}

	if (IsRunningDedicatedServer())
	{
		if (USkeletalMeshComponent* CharacterMesh = CharacterOwner->GetMesh())
		{
			CharacterMesh->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
		}

		if (WeaponMeshComponent)
		{
			WeaponMeshComponent->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
		}
	}
}

void UNpcAttackLogicWeaponSweep::ApplyHitsFromSweep(TArray<FHitResult>& Hits, const FVector& Start, const FVector& End, FCollisionQueryParams& Params, APawn* OwnerPawn)
{
	if (!BattleComponent || !BattleComponent->GetWorld())
	{
		return;
	}

	Hits.Reset();

	if (!BattleComponent->GetWorld()->SweepMultiByChannel(Hits, Start, End, FQuat::Identity, WeaponTraceChannel, FCollisionShape::MakeSphere(WeaponTraceRadius), Params))
	{
		return;
	}

	for (const FHitResult& Hit : Hits)
	{
		AActor* HitActor = Hit.GetActor();
		if (ANpcBase* NpcHit = Cast<ANpcBase>(HitActor))
		{
			if (ANpcBase* OwnerNpc = Cast<ANpcBase>(OwnerPawn))
			{
				if (NpcHit->GetNpcFaction() == OwnerNpc->GetNpcFaction())
				{
					continue;
				}
			}
			continue;
		}

		if (!HitActor || HitActor == OwnerPawn || HitActorsThisAttack.Contains(HitActor))
		{
			continue;
		}

		UGameplayStatics::ApplyDamage(HitActor, BattleComponent->GetDamageAmount(), OwnerPawn->GetController(), OwnerPawn, BattleComponent->GetDamageTypeClass());
		HitActorsThisAttack.Add(HitActor);
	}
}
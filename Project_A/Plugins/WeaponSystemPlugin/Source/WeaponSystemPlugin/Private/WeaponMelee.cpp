


#include "WeaponMelee.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Components/MeshComponent.h"

#include "WeaponPluginLog.h"

namespace {
	static const FName TipSocketName{"TipSocket"};
	static const FName BaseSocketName{"BaseSocket"};
}

AWeaponMelee::AWeaponMelee()
{ 	
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

void AWeaponMelee::BeginPlay()
{
	Super::BeginPlay();

	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &AWeaponMelee::AfterBeginPlay);
}

void AWeaponMelee::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TickAttack(DeltaTime);
}

void AWeaponMelee::AfterBeginPlay()
{   
	if(!MeshComponent)
	{
		UE_LOGFMT(LogWeaponPlugin, Error, "{0}: MeshComponent is null.", FString(__FUNCTION__));
		return;
	}

	if (!AWeaponBase::MeshComponent->DoesSocketExist(::TipSocketName))
	{
		UE_LOGFMT(LogWeaponPlugin, Error, "{0}: Missing socket: {1}", FString(__FUNCTION__), ::TipSocketName.ToString());
	}

	if (!AWeaponBase::MeshComponent->DoesSocketExist(::BaseSocketName))
	{
		UE_LOGFMT(LogWeaponPlugin, Error, "{0}: Missing socket: {1}", FString(__FUNCTION__), ::BaseSocketName.ToString());
		return;
	}

	LastTipPosition = AWeaponBase::MeshComponent->GetSocketLocation(::TipSocketName);
	LastBasePosition = AWeaponBase::MeshComponent->GetSocketLocation(::BaseSocketName); 
}

void AWeaponMelee::TickAttack(float DeltaTime)
{
	if (IsRunningDedicatedServer() || !AWeaponBase::IsWeaponAttacking()) { return; }

	const FVector TipNow = AWeaponBase::MeshComponent->GetSocketLocation(::TipSocketName);
	const FVector BaseNow = AWeaponBase::MeshComponent->GetSocketLocation(::BaseSocketName);

	Server_SendSwingPositions(LastBasePosition, BaseNow, LastTipPosition, TipNow, WeaponRadius);

	LastTipPosition = TipNow;
	LastBasePosition = BaseNow;
}

void AWeaponMelee::Client_StopAttack_Implementation()
{   
	UE_LOGFMT(LogWeaponPlugin, Log, "{0} - called", FString(__FUNCTION__));

	AWeaponBase::SetIsWeaponAttacking(false);  
}

void AWeaponMelee::Client_StartAttack_Implementation()
{
	UE_LOGFMT(LogWeaponPlugin, Log, "{0} - called", FString(__FUNCTION__));

	AWeaponBase::SetIsWeaponAttacking(true);
	LastTipPosition = AWeaponBase::MeshComponent->GetSocketLocation(::TipSocketName);
	LastBasePosition = AWeaponBase::MeshComponent->GetSocketLocation(::BaseSocketName);
}

void AWeaponMelee::Client_DrawDebug_Implementation(FVector LastBase, FVector BaseNow, FVector LastTip, FVector TipNow, float Radius)
{
#if WITH_EDITOR
	DrawDebugSphere(GetWorld(), LastTip, Radius, 12, FColor::Red, false, 0.02f);
	DrawDebugSphere(GetWorld(), TipNow, Radius, 12, FColor::Red, false, 0.02f);

	DrawDebugSphere(GetWorld(), LastBase, Radius, 12, FColor::Blue, false, 0.02f);
	DrawDebugSphere(GetWorld(), BaseNow, Radius, 12, FColor::Blue, false, 0.02f);

	DrawDebugLine(GetWorld(), BaseNow, TipNow, FColor::Green, false, 0.02f, 0, 1.f);
#endif
}

void AWeaponMelee::Server_SendSwingPositions_Implementation(const FVector LastBase, const FVector BaseNow, const FVector LastTip, const FVector TipNow, float Radius)
{
	AActor* OwnerActor = GetOwner();
	if (!OwnerActor)
	{
		UE_LOGFMT(LogWeaponPlugin, Warning, "{0} - No owner, rejecting swing positions.", FString(__FUNCTION__));
		return;
	}

	// Check distance from owner to BaseNow
	const FVector OwnerLoc = OwnerActor->GetActorLocation();
	if (FVector::DistSquared(OwnerLoc, BaseNow) > MaxClientPositionDelta * MaxClientPositionDelta)
	{
		UE_LOGFMT(LogWeaponPlugin, Warning, "{0} - Received swing too far from owner, rejecting.", FString(__FUNCTION__));
		return;
	}

	FHitResult Hit{};
	FCollisionQueryParams Params{};
	Params.AddIgnoredActor(GetOwner());
	Params.AddIgnoredActor(this);

	bool bHit = false;

	UWorld* World = GetWorld();
	if (!World) return;

	// Sweep Tip
	if (!bHit && World->SweepSingleByChannel(
		Hit, LastTip, TipNow, FQuat::Identity,
		ECC_Pawn, FCollisionShape::MakeSphere(Radius), Params))
	{
		UE_LOGFMT(LogWeaponPlugin, Log, "{0} - Tip Hit (from client)", FString(__FUNCTION__));
		bHit = true;
	}

	// Sweep Base
	if (!bHit && World->SweepSingleByChannel(
		Hit, LastBase, BaseNow, FQuat::Identity,
		ECC_Pawn, FCollisionShape::MakeSphere(Radius), Params))
	{
		UE_LOGFMT(LogWeaponPlugin, Log, "{0} - Base Hit (from client)", FString(__FUNCTION__));
		bHit = true;
	}

	// Blade's sweep (Base→Tip)
	if (!bHit && World->SweepSingleByChannel(
		Hit, BaseNow, TipNow, FQuat::Identity,
		ECC_Pawn, FCollisionShape::MakeSphere(Radius), Params))
	{
		UE_LOGFMT(LogWeaponPlugin, Log, "{0} - Blade Hit (from client)", FString(__FUNCTION__));
		bHit = true;
	}

	// Editor debug
	Client_DrawDebug(LastBase, BaseNow, LastTip, TipNow, Radius);

	// Apply damage if hit
	if (bHit && Hit.GetActor())
	{
		UE_LOGFMT(LogWeaponPlugin, Log, "{0} - Hit Actor: {1}", FString(__FUNCTION__), *Hit.GetActor()->GetName());
				
		UGameplayStatics::ApplyDamage(
			Hit.GetActor(),
			Damage,
			GetInstigatorController(),
			this,
			UDamageType::StaticClass()
		);
				
		this->Client_StopAttack();
	}
}

//void AWeaponMelee::Client_DrawDebug_Implementation()
//{
//#if WITH_EDITOR
//    const FVector Tip = Mesh->GetSocketLocation(::TipSocketName);
//    const FVector Base = Mesh->GetSocketLocation(::BaseSocketName);
//    const float Radius = 5.f;
//
//    //DrawDebugLine(GetWorld(), Base, Tip, FColor::Red, false, 0.05f, 0, 2.f);
//
//    //DrawDebugSphere(GetWorld(), LastTip, Radius, 12, FColor::Red, false, 0.02f);
//    DrawDebugSphere(GetWorld(), Tip, Radius, 12, FColor::Red, false, 0.02f);
//
//   // DrawDebugSphere(GetWorld(), LastBase, Radius, 12, FColor::Blue, false, 0.02f);
//    DrawDebugSphere(GetWorld(), Base, Radius, 12, FColor::Blue, false, 0.02f);
//
//    DrawDebugLine(GetWorld(), Base, Tip, FColor::Green, false, 0.02f, 0, 1.f);
//#endif
//}
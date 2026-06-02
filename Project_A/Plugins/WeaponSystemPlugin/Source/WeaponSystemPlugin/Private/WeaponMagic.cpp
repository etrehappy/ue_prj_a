


#include "WeaponMagic.h"
#include "Net/UnrealNetwork.h"
#include "WeaponComponent.h"
#include "WeaponPluginLog.h"

namespace {
	static const float ScreenCenterMainOffsetXY = 0.5f;
    static const float ScreenAdditionalOffsetXY = 0.01f;
	static const float TraceDistance = 50000.f;
    static const float MaxDelta = 2000.f;
}

AWeaponMagic::AWeaponMagic()
{
    PrimaryActorTick.bCanEverTick = true;
    bReplicates = true;
}

void AWeaponMagic::BeginPlay()
{
    Super::BeginPlay();
}

void AWeaponMagic::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

}

void AWeaponMagic::Server_SpawnProjectile_Implementation(const FVector AimOrigin, const FVector AimDirection)
{
    if (!OwningCharacter)
    {
        UE_LOGFMT(LogWeaponPlugin, Warning, "{0} - OwningCharacter is null on server", FString(__FUNCTION__));
        return;
    }

    // Step 1.
    const FVector OwnerLoc = OwningCharacter->GetActorLocation();    
    if (FVector::DistSquared(OwnerLoc, AimOrigin) > FMath::Square(::MaxDelta))
    {
        UE_LOGFMT(LogWeaponPlugin, Warning, "{0} - Client aim origin too far from owner, rejecting", FString(__FUNCTION__));
        return;
    }


    // Step 2.
    FVector TraceEnd = ComputeTraceEnd(AimOrigin, AimDirection);
    FHitResult Hit;
    const bool bHitSomething = PerformAimTrace(AimOrigin, TraceEnd, Hit);

    const FVector TargetPoint = bHitSomething ? Hit.ImpactPoint : TraceEnd;

#if WITH_EDITOR
    if (bHitSomething)
    {
        //DrawDebugSphere(GetWorld(), Hit.ImpactPoint, 10.f, 16, FColor::Red, false, 1.f);
    }
#endif

    // Step 3.
    FVector SpawnLocation;
    FRotator SpawnRotation;
    FVector Direction;
    if (!GetProjectileSpawnTransform(SpawnLocation, SpawnRotation, Direction, TargetPoint))
    {
        UE_LOGFMT(LogWeaponPlugin, Warning, "{0} - Failed to compute spawn transform or direction", FString(__FUNCTION__));
        return;
    }

#if WITH_EDITOR
    //DrawDebugLine(GetWorld(), SpawnLocation, SpawnLocation + Direction * 2000.f, FColor::Green, false, 1.f);
#endif

    // Step 4.
    SpawnProjectileAt(SpawnLocation, SpawnRotation, Direction);
}


void AWeaponMagic::SpawnProjectileAt(const FVector& Location, const FRotator& Rotation, const FVector& Direction)
{
    if (!AWeaponBase::ProjectileClass)
    {
        UE_LOGFMT(LogWeaponPlugin, Warning, "{0} - ProjectileClass is null", FString(__FUNCTION__));
        return;
    }

    UWorld* World = GetWorld();
    if (!World) return;

    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = GetOwner();
    SpawnParams.Instigator = OwningCharacter;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    AProjectileBase* SpawnedProjectile = World->SpawnActor<AProjectileBase>(AWeaponBase::ProjectileClass, Location, Rotation, SpawnParams);
    if (!SpawnedProjectile)
    {
        UE_LOGFMT(LogWeaponPlugin, Warning, "{0} - Failed to spawn projectile", FString(__FUNCTION__));
        return;
    }

    if (SpawnedProjectile->GetMovementComponent())
    {
        SpawnedProjectile->GetMovementComponent()->Velocity = Direction * SpawnedProjectile->GetMovementComponent()->InitialSpeed;
    }
}

bool AWeaponMagic::GetProjectileSpawnTransform(FVector& OutLocation, FRotator& OutRotation, FVector& OutDirection, const FVector& TargetPoint) const
{
    if (!OwningCharacter) return false;

    UWeaponComponent* OwnerWeaponComponent = OwningCharacter->GetComponentByClass<UWeaponComponent>();
    if (!OwnerWeaponComponent) return false;

    USkeletalMeshComponent* Mesh = OwningCharacter->GetMesh();
    if (!Mesh) return false;

    const FName SocketName = OwnerWeaponComponent->GetProjectileSocketName();
    if (!Mesh->DoesSocketExist(SocketName)) return false;

	const FVector HandLocation = Mesh->GetSocketLocation(SocketName) + FVector(0.f, 0.f, 45.f); // temporary offset up
    OutDirection = (TargetPoint - HandLocation).GetSafeNormal();
    if (!OutDirection.IsNearlyZero())
    {
        OutRotation = OutDirection.Rotation();
        OutLocation = HandLocation;
        return true;
    }

    return false;
}

bool AWeaponMagic::PerformAimTrace(const FVector& WorldOrigin, const FVector& TraceEnd, FHitResult& OutHit) const
{
    UWorld* World = GetWorld();
    if (!World) return false;

    FCollisionQueryParams Params{};
    if (OwningCharacter)
    {
        Params.AddIgnoredActor(OwningCharacter);
        if (OwningCharacter->GetMesh())
        {
            Params.AddIgnoredComponent(OwningCharacter->GetMesh());
        }
    }
    Params.bTraceComplex = false;

    return World->LineTraceSingleByChannel(OutHit, WorldOrigin, TraceEnd, ECC_Visibility, Params);
}

FVector AWeaponMagic::ComputeTraceEnd(const FVector& WorldOrigin, const FVector& WorldDirection) const
{
    return WorldOrigin + WorldDirection * ::TraceDistance;
}

bool AWeaponMagic::GetPlayerAim(FVector& OutWorldOrigin, FVector& OutWorldDirection) const
{
    if (!OwningCharacter) return false;

    APlayerController* PC = Cast<APlayerController>(OwningCharacter->GetController());
    if (!PC) return false;

    int32 SizeX = 0, SizeY = 0;
    PC->GetViewportSize(SizeX, SizeY);
    if (SizeX <= 0 || SizeY <= 0) return false;

    FVector2D ScreenCenter(SizeX * ::ScreenCenterMainOffsetXY, SizeY * ::ScreenCenterMainOffsetXY);
    ScreenCenter.Y -= SizeY * ::ScreenAdditionalOffsetXY;
    ScreenCenter.X += SizeX * ::ScreenAdditionalOffsetXY;

    return PC->DeprojectScreenPositionToWorld(ScreenCenter.X, ScreenCenter.Y, OutWorldOrigin, OutWorldDirection);
}


void AWeaponMagic::SpawnProjectile()
{
    UE_LOGFMT(LogWeaponPlugin, Log, "{0} - called", FString(__FUNCTION__));

    if (!AWeaponBase::ProjectileClass)
    {
        UE_LOGFMT(LogWeaponPlugin, Warning, "{0} - Projectile = nullptr", FString(__FUNCTION__));
        return;
    }

    if(!OwningCharacter)
    {
        UE_LOGFMT(LogWeaponPlugin, Warning, "{0} - OwningCharacter = nullptr", FString(__FUNCTION__));
        return;
    }

    // Step 1.
    FVector WorldOrigin{}, WorldDirection{};
    if (!GetPlayerAim(WorldOrigin, WorldDirection))
    {
        UE_LOGFMT(LogWeaponPlugin, Warning, "{0} - Failed to get player aim", FString(__FUNCTION__));
        return;
    }

    // Step 2.
    Server_SpawnProjectile(WorldOrigin, WorldDirection);

    // Step 3.
#if WITH_EDITOR
    const FVector TraceEnd = ComputeTraceEnd(WorldOrigin, WorldDirection);
    //DrawDebugLine(GetWorld(), WorldOrigin, TraceEnd, FColor::Green, false, 1.f);
#endif

}

//FActorSpawnParameters SpawnParams{};
//SpawnParams.Owner = GetOwner();
//SpawnParams.Instigator = OwningCharacter;
//
//UWeaponComponent* OwnerWeaponComponent = OwningCharacter->GetComponentByClass<UWeaponComponent>();
//USkeletalMeshComponent* Mesh = OwningCharacter->GetMesh();
//const FName SocketName = OwnerWeaponComponent->GetProjectileSocketName();
//
//if (!Mesh->DoesSocketExist(SocketName))
//{
//    UE_LOGFMT(LogWeaponPlugin, Warning,
//        "{0} - Socket {1} does not exist on OwningCharacter Mesh",
//        FString(__FUNCTION__), SocketName.ToString());
//    return;
//}
//
//// Делаем трассировку, чтобы понять куда стреляет игрок
//FVector TraceEnd = WorldOrigin + WorldDirection * ::TraceDistance;
//
//FHitResult Hit;
//FCollisionQueryParams Params;
//Params.AddIgnoredActor(OwningCharacter);
//Params.AddIgnoredComponent(OwningCharacter->GetMesh());
//Params.bTraceComplex = false;
//
//
//bool bHitSomething =
//GetWorld()->LineTraceSingleByChannel(
//    Hit,
//    WorldOrigin,
//    TraceEnd,
//    ECC_Visibility,
//    Params
//);
//
//FVector HandLocation = Mesh->GetSocketLocation(SocketName);
//const FVector Direction = (TraceEnd - HandLocation).GetSafeNormal();
//
//if (bHitSomething)
//{
//    DrawDebugSphere(
//        GetWorld(),
//        Hit.ImpactPoint,
//        10.f,
//        16,
//        FColor::Red,
//        false,
//        1.f
//    );
//
//    DrawDebugLine(GetWorld(), HandLocation, HandLocation + Direction * 2000.f, FColor::Green, false, 1.f);
//
//
//    TraceEnd = Hit.ImpactPoint;
//}
//
//const FRotator SpawnRotation = Direction.Rotation();
//
//const FVector SpawnLocation = Mesh->GetSocketLocation(SocketName);
//AProjectileBase* SpawnedProjectile = GetWorld()->SpawnActor<AProjectileBase>(AWeaponBase::ProjectileClass, SpawnLocation, SpawnRotation, SpawnParams);
//SpawnedProjectile->Movement->Velocity = Direction * SpawnedProjectile->Movement->InitialSpeed;

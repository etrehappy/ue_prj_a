


#include "WeaponThrowable.h"
#include "TargetIndicator.h"
//#include "AreaExplosion.h"
#include "Net/UnrealNetwork.h"
//#include "WeaponComponent.h"
#include "ProjectileBase.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
//#include "Kismet/GameplayStatics.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Pawn.h"

#include "WeaponPluginLog.h"

namespace {
	// temporary constants for aiming calculations
    static const float AimDistance = 3000.f;
    static const float SideOffset = 100.f;
}

AWeaponThrowable::AWeaponThrowable()
{
	PrimaryActorTick.bCanEverTick = true;
    SetActorTickEnabled(false);
    bReplicates = true;
}

void AWeaponThrowable::BeginPlay()
{
    Super::BeginPlay();
    UE_LOGFMT(LogWeaponPlugin, Log, "{0} - called", FString(__FUNCTION__));
}

void AWeaponThrowable::StopAiming()
{
    if (IsRunningDedicatedServer())
    {
        UE_LOGFMT(LogWeaponPlugin, Log, "{0} - is called on the server", FString(__FUNCTION__));

        if (bIsAiming)
        {
            Client_StopAiming();
        }
    }
    else
    {
         UE_LOGFMT(LogWeaponPlugin, Log, "{0} - is called on the client, skipping", FString(__FUNCTION__));
	}
 
}

void AWeaponThrowable::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);


    if (!IsRunningDedicatedServer())
    {
        this->UpdateIndicator();
    }
}


void AWeaponThrowable::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    /*DOREPLIFETIME(AWeaponThrowable, ExplosionRadius);*/
    //DOREPLIFETIME(AWeaponThrowable, InitialSpeed);
    //DOREPLIFETIME(AWeaponThrowable, GravityZ);
    //DOREPLIFETIME(AWeaponThrowable, Indicator);
    DOREPLIFETIME(AWeaponThrowable, bIsAiming);
    /*DOREPLIFETIME(AWeaponThrowable, CachedStartLocation);
    DOREPLIFETIME(AWeaponThrowable, CachedInitialVelocity);
    DOREPLIFETIME(AWeaponThrowable, LastAimLocation);
    DOREPLIFETIME(AWeaponThrowable, bIsLastAimValid);*/
}

void AWeaponThrowable::SpawnProjectile()
{
    UE_LOGFMT(LogWeaponPlugin, Log, "{0} - called", FString(__FUNCTION__));

    if (!OwningCharacter)
    {
        UE_LOGFMT(LogWeaponPlugin, Warning, "{0} - OwningCharacter is null", FString(__FUNCTION__));
        return;
    }

    if (!MeshComponent)
    {
        UE_LOGFMT(LogWeaponPlugin, Warning, "{0} - MeshComponent is null", FString(__FUNCTION__));
		return;
    }

    MeshComponent->SetVisibility(false);
    Client_StartAttack();
}

void AWeaponThrowable::StartAiming()
{
    UE_LOGFMT(LogWeaponPlugin, Log, "{0} - called", FString(__FUNCTION__));
       
    Client_StartAiming();
}

bool AWeaponThrowable::IsAiming() const
{
    return bIsAiming;
}

void AWeaponThrowable::SpawnIndicatorIfNeeded()
{
    UE_LOGFMT(LogWeaponPlugin, Log, "{0} - called", FString(__FUNCTION__));

    if (Indicator)
    {
        UE_LOGFMT(LogWeaponPlugin, Log, "{0} - Indicator already exists", FString(__FUNCTION__));
        return;
    }
        
    if(!AWeaponBase::IndicatorClass)
    {
        UE_LOGFMT(LogWeaponPlugin, Warning, "{0} - IndicatorClass is null", FString(__FUNCTION__));
        return;
    }

    AActor* OwnerActor = OwningCharacter;
    if (!OwnerActor)
    {
        UE_LOGFMT(LogWeaponPlugin, Warning, "{0} - OwnerActor is null", FString(__FUNCTION__));
        return;
    }

    UWorld* World = OwnerActor->GetWorld();
    if (!World)
    {
        UE_LOGFMT(LogWeaponPlugin, Warning, "{0} - World is null", FString(__FUNCTION__));
        return;
    }

    FActorSpawnParameters Params;
    Params.Owner = OwnerActor;
    Params.Instigator = OwnerActor->GetInstigator();

    USkeletalMeshComponent* Mesh = OwningCharacter->GetMesh();
    FVector HandLocation = Mesh->GetSocketLocation("RightHandSocket");
    const FRotator HandRotation = OwningCharacter->GetActorTransform().GetRotation().Rotator();


    Indicator = World->SpawnActor<ATargetIndicator>(AWeaponBase::IndicatorClass, HandLocation, HandRotation, Params);
    Indicator->SetActorHiddenInGame(true);
}

void AWeaponThrowable::UpdateIndicator()
{ 
    //On Tick
    if (!bIsAiming || !Indicator)
    {
        //UE_LOGFMT(LogWeaponPlugin, Warning, "{0} - bIsAiming is false or Indicator is null", FString(__FUNCTION__));
        return;
    }

    AActor* OwnerActor = OwningCharacter;
    if (!OwnerActor)
    {
        UE_LOGFMT(LogWeaponPlugin, Warning, "{0} - OwnerActor is null", FString(__FUNCTION__));
        return;
    }
        
    APawn* Pawn = Cast<APawn>(OwnerActor);
    if (!Pawn)
    {
        UE_LOGFMT(LogWeaponPlugin, Warning, "{0} - OwnerActor is not a Pawn", FString(__FUNCTION__));
        return;
    }

    UCameraComponent* Cam = Pawn->FindComponentByClass<UCameraComponent>();
    if (!Cam)
    {
        UE_LOGFMT(LogWeaponPlugin, Warning, "{0} - CameraComponent not found", FString(__FUNCTION__));
        return;
    }


    USkeletalMeshComponent* Mesh = OwningCharacter->GetMesh();
	FVector HandLocation = Mesh->GetSocketLocation("RightHandSocket"); // temporary direct socket name
    const FRotator HandRotation = OwningCharacter->GetActorTransform().GetRotation().Rotator();

    FVector AimPoint = HandLocation + Cam->GetForwardVector() * ::AimDistance;    
    FVector DesiredAimPoint = AimPoint + Cam->GetRightVector() * ::SideOffset;
    FVector Dir = (DesiredAimPoint - HandLocation).GetSafeNormal();
    
    CachedStartLocation = HandLocation;
    CachedInitialVelocity = Dir * InitialSpeed;

    Indicator->UpdateIndicator(HandLocation, CachedInitialVelocity, GravityZ, ExplosionRadius);

    LastAimLocation = Indicator->GetFinalHitLocation();
    bIsLastAimValid = true;

    Server_UpdateBallisticData(CachedStartLocation, CachedInitialVelocity, LastAimLocation, bIsLastAimValid);
}

void AWeaponThrowable::Server_UpdateBallisticData_Implementation(FVector InCachedStartLocation, FVector InCachedInitialVelocity, FVector InLastAimLocation, bool bInIsLastAimValid)
{
    // On tick
    CachedStartLocation = InCachedStartLocation;
    CachedInitialVelocity = InCachedInitialVelocity;
    LastAimLocation = InLastAimLocation;
    bIsLastAimValid = bInIsLastAimValid;
}

void AWeaponThrowable::Client_StartAiming_Implementation()
{
    UE_LOGFMT(LogWeaponPlugin, Log, "{0} - called", FString(__FUNCTION__));   

    Server_SetIsAiming(true);
    SpawnIndicatorIfNeeded();

    if (Indicator)
    {
        UE_LOGFMT(LogWeaponPlugin, Log, "{0} - Showing Indicator", FString(__FUNCTION__));
        Indicator->Show();
        this->SetActorTickEnabled(true);
    }
    else
    {
        UE_LOGFMT(LogWeaponPlugin, Warning, "{0} - Indicator is null", FString(__FUNCTION__));
    }
}


void AWeaponThrowable::Client_StopAiming_Implementation()
{
    UE_LOGFMT(LogWeaponPlugin, Log, "{0} - called", FString(__FUNCTION__));

    Server_SetIsAiming(false);

    if (Indicator)
    {
        Indicator->Hide();
        this->SetActorTickEnabled(false);
    }
    else
    {
        UE_LOGFMT(LogWeaponPlugin, Warning, "{0} - Indicator is null", FString(__FUNCTION__));
    }
}


void AWeaponThrowable::Client_StartAttack_Implementation()
{
    UE_LOGFMT(LogWeaponPlugin, Log, "{0} - called", FString(__FUNCTION__));
    Server_RequestFireAtLocation(LastAimLocation);
}

void AWeaponThrowable::Server_RequestFireAtLocation_Implementation(const FVector& Target)
{
    UE_LOGFMT(LogWeaponPlugin, Log, "{0} - called", FString(__FUNCTION__));

    if (!bIsAiming)
    {
        UE_LOGFMT(LogWeaponPlugin, Warning, "{0} - Not aiming, cannot fire", FString(__FUNCTION__));
        return;
    }

    Client_StopAiming();
    ExecuteFire(Target);
}

void AWeaponThrowable::Server_SetIsAiming_Implementation(bool bNewIsAiming)
{
    UE_LOGFMT(LogWeaponPlugin, Log, "{0} - called", FString(__FUNCTION__));
    bIsAiming = bNewIsAiming;
}

void AWeaponThrowable::ExecuteFire(const FVector& Target)
{
    UE_LOGFMT(LogWeaponPlugin, Log, "{0} - called", FString(__FUNCTION__));

    AActor* OwnerActor = OwningCharacter;
    if (!OwnerActor)
    {
        UE_LOGFMT(LogWeaponPlugin, Warning, "{0} - OwnerActor is null", FString(__FUNCTION__));
        return;
    }

    UWorld* World = OwnerActor->GetWorld();
    if (!World)
    {
        UE_LOGFMT(LogWeaponPlugin, Warning, "{0} - World is null", FString(__FUNCTION__));
        return;
    }
       
    if (!ProjectileClass)
    {
        UE_LOGFMT(LogWeaponPlugin, Warning, "{0} - ProjectileClass is null", FString(__FUNCTION__));
        return;
    }

    const FRotator HandRotation = OwningCharacter->GetActorTransform().GetRotation().Rotator();
    FVector StartLocation = CachedStartLocation;
    FVector LaunchVelocity = CachedInitialVelocity;
    if (LaunchVelocity.IsZero())
    {        
        FVector Dir = (Target - StartLocation).GetSafeNormal();
        LaunchVelocity = Dir * InitialSpeed;

        UE_LOGFMT(LogWeaponPlugin, Warning, "{0} - LaunchVelocity was zero, fallback to simple direction. InitialSpeed =  {1}", FString(__FUNCTION__), InitialSpeed);
    }

    FActorSpawnParameters Params;
    Params.Owner = OwnerActor;
    Params.Instigator = OwnerActor->GetInstigator();

    AProjectileBase* SpawnedProjectile = World->SpawnActor<AProjectileBase>(ProjectileClass, StartLocation, HandRotation, Params);

    if (SpawnedProjectile)
    {     
        SpawnedProjectile->InitializeFromLaunch(LaunchVelocity, GravityZ);
        UE_LOGFMT(LogWeaponPlugin, Log, "{0} - Spawned projectile", FString(__FUNCTION__));
    }
    else
    {
        UE_LOGFMT(LogWeaponPlugin, Warning, "{0} - Failed to spawn projectile, fallback to immediate explosion", FString(__FUNCTION__)); 
    }

}



//FVector AWeaponThrowable::GetLastAimLocation() const
//{
//    return LastAimLocation;
//}
//
//bool AWeaponThrowable::IsLastAimValid() const
//{
//    return bIsLastAimValid;
//}

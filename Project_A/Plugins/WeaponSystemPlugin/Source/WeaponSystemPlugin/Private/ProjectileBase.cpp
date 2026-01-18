


#include "ProjectileBase.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"

#include "WeaponPluginLog.h"


AProjectileBase::AProjectileBase()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
    SetReplicateMovement(true);

	Movement = CreateDefaultSubobject<UProjectileMovementComponent>("Movement");
	Movement->InitialSpeed = 1000.f;
	Movement->MaxSpeed = 1000.f;
	Movement->bShouldBounce = false;

    if (Movement)
    {
        Movement->SetIsReplicated(true);
    }

    Collision = CreateDefaultSubobject<USphereComponent>("Collision");
    Collision->SetSphereRadius(15.f);
	Collision->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	Collision->SetIsReplicated(true);

    SetRootComponent(Collision);
}

void AProjectileBase::BeginPlay()
{
	Super::BeginPlay();

	if (!Collision)
	{
        UE_LOGFMT(LogWeaponPlugin, Warning, "{0} - Collision = nullptr", FString(__FUNCTION__));
	}

    AActor* OwnerActor = GetOwner();

	if (!OwnerActor)
	{
        UE_LOGFMT(LogWeaponPlugin, Warning, "{0} - OwnerActor = nullptr", FString(__FUNCTION__));
	}

    Collision->MoveIgnoreActors.AddUnique(OwnerActor);
	Collision->IgnoreActorWhenMoving(OwnerActor, true);
}

void AProjectileBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AProjectileBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AProjectileBase, ExplosionRadius);
    DOREPLIFETIME(AProjectileBase, ExplosionClass);

}

USphereComponent* AProjectileBase::GetCollisionComponent() const
{
    return Collision;
}

UProjectileMovementComponent* AProjectileBase::GetMovementComponent() const
{
    return Movement;
}

void AProjectileBase::InitializeFromLaunch(const FVector& LaunchVelocity, float GravityZOverride)
{
    if (!Movement)
    {
        UE_LOGFMT(LogWeaponPlugin, Warning, "{0} - Movement = nullptr", FString(__FUNCTION__));
        return;
    }
    
    const float LaunchSpeed = LaunchVelocity.Size();
    Movement->InitialSpeed = LaunchSpeed;
    Movement->MaxSpeed = FMath::Max(Movement->MaxSpeed, LaunchSpeed);
    
    // Compute gravity scale: guard against zero world gravity.
    float WorldGravityZ = -980.f;
    if (UWorld* World = GetWorld())
    {
        WorldGravityZ = World->GetGravityZ();
    }
    
    if (!FMath::IsNearlyZero(WorldGravityZ) && !FMath::IsNearlyZero(GravityZOverride))
    {
        Movement->ProjectileGravityScale = GravityZOverride / WorldGravityZ;
    }
    else
    {
        Movement->ProjectileGravityScale = 1.0f;
    }
    
    Movement->Velocity = LaunchVelocity;
}

void AProjectileBase::Explode(const FHitResult& Hit)
{
    if (!ExplosionClass)
    {
        UE_LOGFMT(LogWeaponPlugin, Warning, "{0} - ExplosionClass = nullptr", FString(__FUNCTION__));
		return;
    }

    FVector SpawnLocation{};
    if (!Hit.ImpactPoint.IsZero())
    {
        SpawnLocation = FVector(Hit.ImpactPoint);
    }
    else
    {
        SpawnLocation = GetActorLocation();
    }

    FActorSpawnParameters Params;
    Params.Owner = GetOwner();
    Params.Instigator = GetInstigator();

    AAreaExplosion* Expl = GetWorld()->SpawnActor<AAreaExplosion>(ExplosionClass, SpawnLocation, FRotator::ZeroRotator, Params);
    if (Expl)
    {
        Expl->Initialize(ExplosionRadius);
        Expl->Explode();
    }
    else
    {
        UE_LOGFMT(LogWeaponPlugin, Warning, "{0} - Failed to spawn AreaExplosion", FString(__FUNCTION__));
    }
    
}

void AProjectileBase::NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
    if (Other && Other == GetOwner())
    {
        UE_LOGFMT(LogWeaponPlugin, Verbose, "{0} - Hit owner; ignoring", FString(__FUNCTION__));
        return;
    }

    OnProjectileImpact(Hit);

    if (HasAuthority())
    {

        if (bIsItAoe)
        {
            Explode(Hit);
        }
        else
        {
            UGameplayStatics::ApplyDamage(
                Other,
                Damage,
                GetInstigatorController(),
                this,
                DamageTypeClass
            );
        }
		
        Destroy();
    }
    else
    {
        UE_LOGFMT(LogWeaponPlugin, Log, "{0} - No authority to spawn explosion", FString(__FUNCTION__));
    }    
}






#include "AreaExplosion.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

#include "WeaponPluginLog.h"


AAreaExplosion::AAreaExplosion()
{
	PrimaryActorTick.bCanEverTick = true;
    bReplicates = true;

	ParticleComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("FX"));
	RootComponent = ParticleComponent;
}

void AAreaExplosion::Initialize(float InRadius)
{
    UE_LOGFMT(LogWeaponPlugin, Log, "{0} - called", FString(__FUNCTION__));
	Radius = InRadius;
}

void AAreaExplosion::Explode()
{
    UE_LOGFMT(LogWeaponPlugin, Log, "{0} - called", FString(__FUNCTION__));
    if (!HasAuthority()) return;

    FVector Origin = GetActorLocation();

    UGameplayStatics::ApplyRadialDamageWithFalloff(
        this,
        Damage,
        10.f,
        Origin,
        Radius * 0.2f,
        Radius,
        1.0f,
        DamageType,
        {},
        this,
        GetInstigatorController()
    );

    Multicast_PlayEffects();

    SetLifeSpan(2.0f);
}
 
void AAreaExplosion::BeginPlay()
{
    UE_LOGFMT(LogWeaponPlugin, Log, "{0} - called", FString(__FUNCTION__));
	Super::BeginPlay();
	
}

void AAreaExplosion::Multicast_PlayEffects_Implementation()
{
    UE_LOGFMT(LogWeaponPlugin, Log, "{0} - called", FString(__FUNCTION__));
    if (ParticleComponent)
        ParticleComponent->Activate(true);
}

void AAreaExplosion::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}



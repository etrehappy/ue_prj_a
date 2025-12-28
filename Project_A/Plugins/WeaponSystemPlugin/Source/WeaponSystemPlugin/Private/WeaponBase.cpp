
#include "WeaponBase.h"
#include "WeaponDataAsset.h"
#include "Net/UnrealNetwork.h"
#include "WeaponPluginLog.h"


AWeaponBase::AWeaponBase()
{ 	
	PrimaryActorTick.bCanEverTick = false;
    bReplicates = true;

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
    SetRootComponent(Mesh);
    Mesh->SetCollisionProfileName(TEXT("BlockAll"));
    Mesh->SetIsReplicated(true);
}

void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	
}



void AWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWeaponBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AWeaponBase, WeaponTag);
    DOREPLIFETIME(AWeaponBase, Damage);
}

void AWeaponBase::InitializeFromData(UWeaponDataAsset* Data)
{
    if (!Data) 
    {
        UE_LOGFMT(LogWeaponPlugin, Warning, "{0} — WeaponDataAsset is null", FString(__FUNCTION__));
        return;
    }

    if (!Mesh) 
    {
        UE_LOGFMT(LogWeaponPlugin, Warning, "{0} — Mesh component is null", FString(__FUNCTION__));
        return;
    }

    if (!HasAuthority())
    {   
        UE_LOGFMT(LogWeaponPlugin, Warning, "{0} — No authority to initialize weapon from data", FString(__FUNCTION__));
        return;
    }

    WeaponTag = Data->WeaponTag;
    Damage = Data->Damage;

    if (!Data->StaticMesh)
    {        
        UE_LOGFMT(LogWeaponPlugin, Warning, "{0} — StaticMesh in WeaponDataAsset is null", FString(__FUNCTION__));
    }

    Multicast_SetStaticMesh(Data->StaticMesh);

    //AttackAnimation = Data->AttackAnimation;
    //HitEffect = Data->HitEffect;
    //SwingSound = Data->SwingSound;

    UE_LOGFMT(LogWeaponPlugin, Log, "{0} — Weapon initialized from data: {1}", FString(__FUNCTION__), *Data->GetName());
}

void AWeaponBase::Multicast_SetCollision_Implementation(ECollisionEnabled::Type CollisionEnabled, ECollisionResponse CollisionResponse)
{
    if (!Mesh)
    {
        UE_LOGFMT(LogWeaponPlugin, Warning, "{0} — Mesh component is null", FString(__FUNCTION__));
        return;
    }

    Mesh->SetCollisionEnabled(CollisionEnabled);
    Mesh->SetCollisionResponseToAllChannels(CollisionResponse);
}

const FGameplayTag& AWeaponBase::GetWeaponTag() const
{
    return WeaponTag;
}

//void AWeaponBase::Server_InitializeFromData_Implementation(UWeaponDataAsset* Data)
//{
//
//}

void AWeaponBase::Multicast_SetStaticMesh_Implementation(UStaticMesh* NewMesh)
{
    if (!Mesh || !NewMesh)
    {
        UE_LOGFMT(LogWeaponPlugin, Warning, "{0} — Mesh component or NewMesh is null", FString(__FUNCTION__));
        return;
    }

    Mesh->SetStaticMesh(NewMesh);    
}


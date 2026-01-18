
#include "WeaponBase.h"
#include "WeaponDataAsset.h"
#include "Net/UnrealNetwork.h"
#include "WeaponPluginLog.h"


AWeaponBase::AWeaponBase()
{ 	
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	SetRootComponent(RootSceneComponent);

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticComponent"));
	StaticMeshComponent->SetupAttachment(RootSceneComponent);

	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalComponent"));
	SkeletalMeshComponent->SetupAttachment(RootSceneComponent);
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
	DOREPLIFETIME(AWeaponBase, MeshComponent);
	DOREPLIFETIME(AWeaponBase, ProjectileClass);
	DOREPLIFETIME(AWeaponBase, OwningCharacter);
	DOREPLIFETIME(AWeaponBase, AttackRange);
	DOREPLIFETIME(AWeaponBase, IndicatorClass);
	DOREPLIFETIME(AWeaponBase, bIsWeaponAttacking);
	DOREPLIFETIME(AWeaponBase, DamageTypeClass);
}

bool AWeaponBase::InitialiseMeshComponent(UStreamableRenderAsset* NewMesh)
{
	UE_LOGFMT(LogWeaponPlugin, Log, "{0} - called", FString(__FUNCTION__));

	if (!NewMesh)
	{
		UE_LOGFMT(LogWeaponPlugin, Warning, "{0} - NewMesh is null", FString(__FUNCTION__));
		return false;
	}

	if (auto* NewStaticMesh = Cast<UStaticMesh>(NewMesh))
	{		
		StaticMeshComponent->SetStaticMesh(NewStaticMesh);
		SkeletalMeshComponent->DestroyComponent();
		MeshComponent = StaticMeshComponent;

		UE_LOGFMT(LogWeaponPlugin, Log, "{0} - StaticMesh was set", FString(__FUNCTION__));
	}
	else if (auto* NewSkeletalMesh = Cast<USkeletalMesh>(NewMesh))
	{		
		SkeletalMeshComponent->SetSkeletalMesh(NewSkeletalMesh);
		StaticMeshComponent->DestroyComponent();
		MeshComponent = SkeletalMeshComponent;

		UE_LOGFMT(LogWeaponPlugin, Log, "{0} - SkeletalMesh was set", FString(__FUNCTION__));
	}

	if (!MeshComponent)
	{
		UE_LOGFMT(LogWeaponPlugin, Warning, "{0} - Failed to create MeshComponent", FString(__FUNCTION__));
		return false;
	}
		
	MeshComponent->SetCollisionProfileName(TEXT("BlockAll"));
	MeshComponent->SetIsReplicated(true);

	return true;
}

void AWeaponBase::InitialiseFromData(UWeaponDataAsset* Data)
{
	if (!Data)
	{
		UE_LOGFMT(LogWeaponPlugin, Warning, "{0} - WeaponDataAsset is null", FString(__FUNCTION__));
		return;
	}

	if (!HasAuthority())
	{
		UE_LOGFMT(LogWeaponPlugin, Warning, "{0} - No authority to initialize weapon from data", FString(__FUNCTION__));
		return;
	}

	OwningCharacter = GetOwner<ACharacter>();
	if (!OwningCharacter)
	{
		UE_LOGFMT(LogWeaponPlugin, Warning, "{0} - Owning character is null", FString(__FUNCTION__));
	}

	WeaponTag = Data->GetWeaponTag();
	Damage = Data->GetDamage();
	AttackRange = Data->GetAttackRange();

	if (Data->GetProjectileClass())
	{
		ProjectileClass = Data->GetProjectileClass();
	}
	else if (Data->GetHasProjectile())
	{
		UE_LOGFMT(LogWeaponPlugin, Log, "{0} - ProjectileClass is null in a {1}", FString(__FUNCTION__), Data->GetName());
	}


	if (Data->GetIndicatorClass())
	{
		UE_LOGFMT(LogWeaponPlugin, Log, "{0} - Setting IndicatorClass", FString(__FUNCTION__));
		IndicatorClass = Data->GetIndicatorClass();
	}
	else if (Data->GetDoesSupportAiming())
	{
		UE_LOGFMT(LogWeaponPlugin, Log, "{0} - IndicatorClass is null in a {1}", FString(__FUNCTION__), Data->GetName());
	}

	if (Data->GetMesh())
	{
		InitialiseMeshComponent(Data->GetMesh());
	}
	else
	{
		UE_LOGFMT(LogWeaponPlugin, Warning, "{0} - Mesh is null in a {1}", FString(__FUNCTION__), Data->GetName());
	}

	if (Data->GetDamageTypeClass())
	{		
		DamageTypeClass = Data->GetDamageTypeClass();
	}
	else
	{
		UE_LOGFMT(LogWeaponPlugin, Warning, "{0} - DamageType is null in a {1}", FString(__FUNCTION__), Data->GetName());
	}
	

	UE_LOGFMT(LogWeaponPlugin, Log, "{0} - Weapon initialized from data: {1}", FString(__FUNCTION__), *Data->GetName());
}

void AWeaponBase::StartAttack()
{
	Client_StartAttack();
}

void AWeaponBase::StopAttack()
{
	Client_StopAttack();
}

const FGameplayTag& AWeaponBase::GetWeaponTag() const
{
	return WeaponTag;
}

bool AWeaponBase::IsWeaponAttacking() const
{
	return bIsWeaponAttacking;
}

void AWeaponBase::SetIsWeaponAttacking(bool bNewIsWeaponAttacking)
{
	Server_SetIsWeaponAttacking(bNewIsWeaponAttacking);
}

void AWeaponBase::SetCollision(ECollisionEnabled::Type CollisionEnabled, ECollisionResponse CollisionResponse)
{
	if (!IsRunningDedicatedServer())
	{
		UE_LOGFMT(LogWeaponPlugin, Warning, "{0} - This function should only be called on the server", FString(__FUNCTION__));
		return;
	}

	Multicast_SetCollision(CollisionEnabled, CollisionResponse);
}

void AWeaponBase::Multicast_SetCollision_Implementation(ECollisionEnabled::Type CollisionEnabled, ECollisionResponse CollisionResponse)
{
	UE_LOGFMT(LogWeaponPlugin, Log, "{0} - called", FString(__FUNCTION__));
		
	if (!MeshComponent)
	{
		UE_LOGFMT(LogWeaponPlugin, Warning, "{0} - Mesh component is null", FString(__FUNCTION__));
		return;
	}

	MeshComponent->SetCollisionEnabled(CollisionEnabled);
	MeshComponent->SetCollisionResponseToAllChannels(CollisionResponse);
}

void AWeaponBase::Client_StartAttack_Implementation()
{
	UE_LOGFMT(LogWeaponPlugin, Log, "{0} - called", FString(__FUNCTION__));
}

void AWeaponBase::Client_StopAttack_Implementation()
{
	UE_LOGFMT(LogWeaponPlugin, Log, "{0} - called", FString(__FUNCTION__));
}


void AWeaponBase::Server_SetIsWeaponAttacking_Implementation(bool bNewIsWeaponAttacking)
{
	bIsWeaponAttacking = bNewIsWeaponAttacking;
}




//void AWeaponBase::Server_InitializeFromData_Implementation(UWeaponDataAsset* Data)
//{
//
//}

//void AWeaponBase::Multicast_SetMesh_Implementation(UStreamableRenderAsset* NewMesh)
//{
//	UE_LOGFMT(LogWeaponPlugin, Log, "{0} - called", FString(__FUNCTION__));
//
//	if (!IsRunningDedicatedServer())
//	{
//		UE_LOGFMT(LogWeaponPlugin, Log, "{0} - called on a client, skipping", FString(__FUNCTION__));
//		return;
//	}
//
//	if (!NewMesh)
//	{
//		UE_LOGFMT(LogWeaponPlugin, Warning, "{0} - Mesh component or NewMesh is null", FString(__FUNCTION__));
//		return;
//	}
//
//	if (!NewMesh)
//	{
//		UE_LOGFMT(LogWeaponPlugin, Warning, "{0} - NewMesh is null", FString(__FUNCTION__));
//		return;
//	}
//
//	if (auto* SM = Cast<UStaticMesh>(NewMesh))
//	{
//		UE_LOGFMT(LogWeaponPlugin, Log, "{0} - Setting Static Mesh", FString(__FUNCTION__));
//		StaticMeshComponent->SetStaticMesh(SM);
//		SkeletalMeshComponent->DestroyComponent();
//		MeshComponent = StaticMeshComponent;
//	}
//	else if (auto* SK = Cast<USkeletalMesh>(NewMesh))
//	{
//		UE_LOGFMT(LogWeaponPlugin, Log, "{0} - Setting Skeletal Mesh", FString(__FUNCTION__));
//
//		SkeletalMeshComponent->SetSkeletalMesh(SK);
//		MeshComponent = SkeletalMeshComponent;
//	}
//
//	if (!MeshComponent)
//	{
//		UE_LOGFMT(LogWeaponPlugin, Error, "{0} - Failed to create MeshComponent", FString(__FUNCTION__));
//		return;
//	}
//}



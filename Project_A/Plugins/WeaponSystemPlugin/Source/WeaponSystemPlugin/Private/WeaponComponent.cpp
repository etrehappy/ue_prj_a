
#include "WeaponComponent.h"
#include "WeaponFactory.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/Character.h"

#include "WeaponPluginLog.h"


UWeaponComponent::UWeaponComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UWeaponComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UWeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UWeaponComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UWeaponComponent, CurrentWeapon);
	DOREPLIFETIME(UWeaponComponent, IsWeaponEquiped);
}

const bool UWeaponComponent::IsWeaponCurrentlyEquiped() const
{
	return IsWeaponEquiped;
}

//AWeaponBase* UWeaponComponent::EquipWeaponbyClass(TSubclassOf<AWeaponBase> WeaponClass)
//{
//	if (!WeaponClass)
//	{
//		UE_LOGFMT(LogWeaponPlugin, Warning, "{0} — WeaponClass = nullptr", FString(__FUNCTION__));
//		return nullptr;
//	}
//
//	if (CurrentWeapon)
//	{
//		CurrentWeapon->Destroy();
//		CurrentWeapon = nullptr;
//	}
//
//	FActorSpawnParameters SpawnParams;
//	SpawnParams.Owner = GetOwner();
//	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
//	CurrentWeapon = GetWorld()->SpawnActor<AWeaponBase>(WeaponClass, SpawnParams);
//
//	return CurrentWeapon;
//}


void UWeaponComponent::EquipWeaponbyTag(const FGameplayTag WeaponTag, const FName AttachSocketName, const FTransform& SpawnTransform, USkeletalMeshComponent* AttachTo, bool bIsCollisionDesabled)
{
	if (!GetWorld() || (GetWorld()->GetNetMode() != NM_Client))
	{
        UE_LOGFMT(LogWeaponPlugin, Warning, "{0} — EquipWeaponbyTag should be called on Client only.", FString(__FUNCTION__));
		return;
	}

	if(!AttachTo)
	{
		UE_LOGFMT(LogWeaponPlugin, Warning, "{0} — Abort. AttachTo is nullptr, weapon will not be attached after spawn.", FString(__FUNCTION__));
		return;
    }

	ACharacter* Character = Cast<ACharacter>(AttachTo->GetOwner());

    if (!Character)
	{
		UE_LOGFMT(LogWeaponPlugin, Warning, "{0} — AttachTo's owner is not ACharacter, aborting.", FString(__FUNCTION__));
		return;
	}


	Server_EquipWeaponByTag(WeaponTag, AttachSocketName, SpawnTransform, Character, bIsCollisionDesabled);

}

void UWeaponComponent::UnequipWeapon()
{
	if (!GetWorld() || (GetWorld()->GetNetMode() != NM_Client))
	{
		UE_LOGFMT(LogWeaponPlugin, Warning, "{0} — UnequipWeapon should be called on Client only.", FString(__FUNCTION__));
		return;
    }
    
	if (!CurrentWeapon)
	{
		UE_LOGFMT(LogWeaponPlugin, Warning, "{0} — CurrentWeapon = nullptr.", FString(__FUNCTION__));
		return;
	}

	Server_UnequipWeapon();
}


void UWeaponComponent::Server_EquipWeaponByTag_Implementation(const FGameplayTag WeaponTag, const FName AttachSocketName, const FTransform SpawnTransform, ACharacter* AttachToCharacter, bool bIsCollisionDesabled)
{
    USceneComponent* AttachTo = AttachToCharacter ? AttachToCharacter->GetMesh() : nullptr;

	if(!AttachTo)
	{
		UE_LOGFMT(LogWeaponPlugin, Warning, "{0} — Abort. AttachTo is nullptr.", FString(__FUNCTION__));
		return;
    }

	if (CurrentWeapon)
	{
		if (CurrentWeapon->GetWeaponTag() == WeaponTag)
		{
			return;
		}

		Server_UnequipWeapon();
	}

	FActorSpawnParameters SpawnParams{};
	SpawnParams.Owner = GetOwner();
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	CurrentWeapon = UWeaponFactory::SpawnWeaponByTag(GetWorld(), WeaponTag, SpawnParams, SpawnTransform);
	if (!CurrentWeapon)
	{
		UE_LOGFMT(LogWeaponPlugin, Warning, "{0} — Failed to spawn weapon with tag: {1}", FString(__FUNCTION__), *WeaponTag.ToString());
		return;
    }

#if UE_BUILD_DEVELOPMENT
	if (CurrentWeapon)
	{
		UE_LOGFMT(LogWeaponPlugin, Log, "{0} — Spawned weapon: {1}", FString(__FUNCTION__), *CurrentWeapon->GetName());
	}
#endif // UE_BUILD_DEVELOPMENT

	IsWeaponEquiped = true;

	//CurrentWeapon->SetReplicates(true);

	if (bIsCollisionDesabled)
	{
		CurrentWeapon->Multicast_SetCollision(ECollisionEnabled::NoCollision, ECR_Ignore);
	}

	CurrentWeapon->AttachToComponent(AttachTo, FAttachmentTransformRules::KeepRelativeTransform, AttachSocketName);

	return;
}

void UWeaponComponent::Server_UnequipWeapon_Implementation()
{
	if (!CurrentWeapon)
	{
		UE_LOGFMT(LogWeaponPlugin, Warning, "{0} — CurrentWeapon = nullptr.", FString(__FUNCTION__));
		return;
	}

	CurrentWeapon->Destroy();
	CurrentWeapon = nullptr;
	IsWeaponEquiped = false;

}


void UWeaponComponent::OnRep_CurrentWeapon()
{
	IsWeaponEquiped = (CurrentWeapon != nullptr);

#if WITH_EDITOR
	if (CurrentWeapon)
	{
		UE_LOGFMT(LogWeaponPlugin, Log, "{0} — CurrentWeapon: {1}", FString(__FUNCTION__), *CurrentWeapon->GetName());
	}
	else
	{
		UE_LOGFMT(LogWeaponPlugin, Log, "{0} — CurrentWeapon: nullptr", FString(__FUNCTION__));
	}
#endif

}



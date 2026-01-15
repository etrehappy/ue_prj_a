
#include "WeaponComponent.h"
#include "WeaponFactory.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/Character.h"
#include "WeaponThrowable.h"

#include "WeaponPluginLog.h"

//////////////////////////////
/// struct FLustWeaponSettings
//////////////////////////////
void FLustWeaponSettings::Update(const FGameplayTag WeaponTag, const FName AttachSocketName, const FTransform& SpawnTransform, ACharacter* AttachTo, bool bIsCollisionDesabled, FName InProjectileSocketName)
{
	this->WeaponTag_m = WeaponTag;
	this->AttachSocketName_m = AttachSocketName;
	this->SpawnTransform_m = SpawnTransform;
	this->AttachTo_m = AttachTo;
	this->bIsCollisionDesabled_m = bIsCollisionDesabled;
	this->ProjectileSocketName_m = InProjectileSocketName;

}

bool FLustWeaponSettings::IsEmpty() const
{
	bool bIsSpawnTransformEmpty = SpawnTransform_m.IsValid();
	bool bIsAttachSocketNameEmpty = AttachSocketName_m.IsNone();
	bool bIsAttachToEmpty = !AttachTo_m;
	bool bIsProjectileSocketNameEmpty = ProjectileSocketName_m.IsNone();

	if (bIsSpawnTransformEmpty || bIsAttachSocketNameEmpty || bIsAttachToEmpty || bIsProjectileSocketNameEmpty)
	{
		UE_LOGFMT(LogWeaponPlugin, Log, "{0} - One of the settings is empty: bIsSpawnTransformEmpty={1}, bIsAttachSocketNameEmpty={2}, bIsAttachToEmpty={3}, bIsProjectileSocketNameEmpty={4}", FString(__FUNCTION__), bIsSpawnTransformEmpty, bIsAttachSocketNameEmpty, bIsAttachToEmpty, bIsProjectileSocketNameEmpty);
		return true;
	}

	return false;
}




//////////////////////////
/// class UWeaponComponent
//////////////////////////

UWeaponComponent::UWeaponComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UWeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UWeaponComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UWeaponComponent, CurrentWeapon);
	DOREPLIFETIME(UWeaponComponent, bIsWeaponEquiped);
	DOREPLIFETIME(UWeaponComponent, ProjectileSocketName);
	DOREPLIFETIME(UWeaponComponent, CurrentThrowableItem);
	DOREPLIFETIME(UWeaponComponent, bIsThrowableItemEquiped);
}

void UWeaponComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

const bool UWeaponComponent::IsCurrentThrowableItemEquiped() const
{
	return bIsThrowableItemEquiped;
}

const bool UWeaponComponent::IsCurrentWeaponEquiped() const
{
	return bIsWeaponEquiped;
}

AWeaponBase* UWeaponComponent::GetAnyActiveEquipedWeapon() const
{
	if (bIsThrowableItemEquiped)
	{
		return CurrentThrowableItem;
	}
	else if (bIsWeaponEquiped)
	{
		return CurrentWeapon;
	}

	return nullptr;
}

AWeaponBase* UWeaponComponent::GetCurrentWeapon() const
{
	return CurrentWeapon;
}

AWeaponThrowable* UWeaponComponent::GetCurrentThrowableItem() const
{
	return CurrentThrowableItem;
}

FName UWeaponComponent::GetProjectileSocketName() const
{
	return ProjectileSocketName;
}

void UWeaponComponent::Attack()
{
	if (CurrentWeapon)
	{
		UE_LOGFMT(LogWeaponPlugin, Log, "{0} - Attacking with weapon: {1}", FString(__FUNCTION__), *CurrentWeapon->GetName());
		CurrentWeapon->StartAttack();
	}
	else if (CurrentThrowableItem)
	{
		UE_LOGFMT(LogWeaponPlugin, Log, "{0} - Attacking with throwable item: {1}", FString(__FUNCTION__), *CurrentThrowableItem->GetName());
		CurrentThrowableItem->StartAttack();
	}
	else
	{
		UE_LOGFMT(LogWeaponPlugin, Warning, "{0} - No weapon is currently equipped.", FString(__FUNCTION__));
		return;
	}
}

void UWeaponComponent::StopAttack()
{
	if (CurrentWeapon)
	{
		UE_LOGFMT(LogWeaponPlugin, Log, "{0} - Stopping attack with weapon: {1}", FString(__FUNCTION__), *CurrentWeapon->GetName());
		CurrentWeapon->StopAttack();
	}
	else if (CurrentThrowableItem)
	{
		UE_LOGFMT(LogWeaponPlugin, Log, "{0} - Stopping attack with throwable item: {1}", FString(__FUNCTION__), *CurrentThrowableItem->GetName());
		CurrentThrowableItem->StopAttack();
		Server_UnequipThrowableItem();
	}
	else
	{
		UE_LOGFMT(LogWeaponPlugin, Warning, "{0} - No weapon is currently equipped.", FString(__FUNCTION__));
	}
}

void UWeaponComponent::UnequipWeapon()
{
	UE_LOGFMT(LogWeaponPlugin, Log, "{0} - called", FString(__FUNCTION__));

	if (!CurrentWeapon)
	{
		UE_LOGFMT(LogWeaponPlugin, Warning, "{0} - CurrentWeapon = nullptr.", FString(__FUNCTION__));
		return;
	}

	Server_UnequipWeapon();
}

void UWeaponComponent::UnequipThrowableItem()
{
	UE_LOGFMT(LogWeaponPlugin, Log, "{0} - called", FString(__FUNCTION__));

	if (!CurrentThrowableItem)
	{
		UE_LOGFMT(LogWeaponPlugin, Log, "{0} - CurrentThrowableItem = nullptr.", FString(__FUNCTION__));
		return;
	}
   
	Server_UnequipThrowableItem();
}

void UWeaponComponent::SetCurrentWeapon(AWeaponBase* NewWeapon, const FGameplayTag WeaponTag, const FName AttachSocketName, const FTransform& SpawnTransform, ACharacter* AttachToCharacter, bool bIsCollisionDesabled, FName InProjectileSocketName)
{
	if(!NewWeapon)
	{
		UE_LOGFMT(LogWeaponPlugin, Warning, "{0} - NewWeapon = nullptr", FString(__FUNCTION__));
		return;
	}

	CurrentWeapon = NewWeapon;
	bIsWeaponEquiped = true;
	LastWeaponSettings.Update(WeaponTag, AttachSocketName, SpawnTransform, AttachToCharacter, bIsCollisionDesabled, InProjectileSocketName);


#if UE_BUILD_DEVELOPMENT
	if (CurrentWeapon)
	{
		UE_LOGFMT(LogWeaponPlugin, Log, "{0} - Spawned weapon: {1}", FString(__FUNCTION__), *CurrentWeapon->GetName());
	}
#endif // UE_BUILD_DEVELOPMENT

	if (bIsCollisionDesabled)
	{
		CurrentWeapon->SetCollision(ECollisionEnabled::NoCollision, ECR_Ignore);
	}

	if (!InProjectileSocketName.IsNone())
	{
		this->ProjectileSocketName = InProjectileSocketName;
		UE_LOGFMT(LogWeaponPlugin, Log, "{0} - Set ProjectileSocketName to: {1}", FString(__FUNCTION__), *InProjectileSocketName.ToString());
	}

	USceneComponent* AttachTo = AttachToCharacter ? AttachToCharacter->GetMesh() : nullptr;

	if (!AttachTo)
	{
		UE_LOGFMT(LogWeaponPlugin, Warning, "{0} - Abort. AttachTo is nullptr.", FString(__FUNCTION__));
		return;
	}
	CurrentWeapon->AttachToComponent(AttachTo, FAttachmentTransformRules::KeepRelativeTransform, AttachSocketName);
}

void UWeaponComponent::SetCurrentThrowableItem(AWeaponThrowable* InThrowableItem, USceneComponent* AttachTo, bool bIsCollisionDesabled, const FName AttachSocketName, FName InProjectileSocketName)
{
	UE_LOGFMT(LogWeaponPlugin, Log, "{0} - called", FString(__FUNCTION__));

	Server_UnequipWeapon();

	if (!InThrowableItem)
	{
		UE_LOGFMT(LogWeaponPlugin, Warning, "{0} - InThrowableItem = nullptr", FString(__FUNCTION__));
		return;
	}

	CurrentThrowableItem = InThrowableItem;
	bIsThrowableItemEquiped = true;


#if UE_BUILD_DEVELOPMENT
	if (CurrentThrowableItem)
	{
		UE_LOGFMT(LogWeaponPlugin, Log, "{0} - Spawned weapon: {1}", FString(__FUNCTION__), *CurrentThrowableItem->GetName());
	}
#endif // UE_BUILD_DEVELOPMENT

	if (bIsCollisionDesabled)
	{
		CurrentThrowableItem->SetCollision(ECollisionEnabled::NoCollision, ECR_Ignore); // TODO check net
	}

	if (!InProjectileSocketName.IsNone())
	{
		this->ProjectileSocketName = InProjectileSocketName;
		UE_LOGFMT(LogWeaponPlugin, Log, "{0} - Set ProjectileSocketName to: {1}", FString(__FUNCTION__), *InProjectileSocketName.ToString());
	}

	CurrentThrowableItem->AttachToComponent(AttachTo, FAttachmentTransformRules::KeepRelativeTransform, AttachSocketName);
	CurrentThrowableItem->StartAiming();
}

void UWeaponComponent::PutLastWeaponBackOn()
{
	if (LastWeaponSettings.IsEmpty())
	{
		UE_LOGFMT(LogWeaponPlugin, Warning, "{0} - LastWeaponSettings is empty, cannot re-equip previous weapon.", FString(__FUNCTION__));
		return;
	}

	Server_EquipWeaponByTag(LastWeaponSettings.WeaponTag_m, LastWeaponSettings.AttachSocketName_m, LastWeaponSettings.SpawnTransform_m, LastWeaponSettings.AttachTo_m, LastWeaponSettings.bIsCollisionDesabled_m, LastWeaponSettings.ProjectileSocketName_m);
}

void UWeaponComponent::EquipWeaponbyTag(const FGameplayTag WeaponTag, const FName AttachSocketName, const FTransform& SpawnTransform, USkeletalMeshComponent* AttachTo, bool bIsCollisionDesabled, FName InProjectileSocketName)
{
	if (!GetWorld() || (GetWorld()->GetNetMode() != NM_Client))
	{
		UE_LOGFMT(LogWeaponPlugin, Warning, "{0} - EquipWeaponbyTag should be called on Client only.", FString(__FUNCTION__));
		return;
	}

	if(!AttachTo)
	{
		UE_LOGFMT(LogWeaponPlugin, Warning, "{0} - Abort. AttachTo is nullptr, weapon will not be attached after spawn.", FString(__FUNCTION__));
		return;
	}

	ACharacter* Character = Cast<ACharacter>(AttachTo->GetOwner());

	if (!Character)
	{
		UE_LOGFMT(LogWeaponPlugin, Warning, "{0} - AttachTo's owner is not ACharacter, aborting.", FString(__FUNCTION__));
		return;
	}

	Server_EquipWeaponByTag(WeaponTag, AttachSocketName, SpawnTransform, Character, bIsCollisionDesabled, InProjectileSocketName);

}

void UWeaponComponent::Server_UnequipWeapon_Implementation()
{
	UE_LOGFMT(LogWeaponPlugin, Log, "{0} - called", FString(__FUNCTION__));

	if (!CurrentWeapon)
	{
		UE_LOGFMT(LogWeaponPlugin, Warning, "{0} - CurrentWeapon = nullptr.", FString(__FUNCTION__));
		return;
	}

	CurrentWeapon->Destroy();
	CurrentWeapon = nullptr;
	bIsWeaponEquiped = false;
}

void UWeaponComponent::Server_UnequipThrowableItem_Implementation()
{
	UE_LOGFMT(LogWeaponPlugin, Log, "{0} - called", FString(__FUNCTION__));

	if (!CurrentThrowableItem)
	{
		UE_LOGFMT(LogWeaponPlugin, Warning, "{0} - CurrentThrowableItem = nullptr.", FString(__FUNCTION__));
		return;
	}

	CurrentThrowableItem->StopAiming();
	CurrentThrowableItem->Destroy();
	CurrentThrowableItem = nullptr;
	bIsThrowableItemEquiped = false;

	PutLastWeaponBackOn();
}

void UWeaponComponent::Server_EquipWeaponByTag_Implementation(const FGameplayTag WeaponTag, const FName AttachSocketName, const FTransform SpawnTransform, ACharacter* AttachToCharacter, bool bIsCollisionDesabled, FName InProjectileSocketName)
{
	UE_LOGFMT(LogWeaponPlugin, Log, "{0} - called", FString(__FUNCTION__));

	USceneComponent* AttachTo = AttachToCharacter ? AttachToCharacter->GetMesh() : nullptr;

	if(!AttachTo)
	{
		UE_LOGFMT(LogWeaponPlugin, Warning, "{0} - Abort. AttachTo is nullptr.", FString(__FUNCTION__));
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

	AWeaponBase* NewWeapon = UWeaponFactory::SpawnWeaponByTag(GetWorld(), WeaponTag, SpawnParams, SpawnTransform);
	if (!NewWeapon)
	{
		UE_LOGFMT(LogWeaponPlugin, Warning, "{0} - Failed to spawn weapon with tag: {1}", FString(__FUNCTION__), *WeaponTag.ToString());
		PutLastWeaponBackOn();
		return;
	}

	if (AWeaponThrowable* NewThrowableItem = Cast<AWeaponThrowable>(NewWeapon))
	{
		SetCurrentThrowableItem(NewThrowableItem, AttachTo, bIsCollisionDesabled, AttachSocketName, InProjectileSocketName);
	}
	else
	{
		SetCurrentWeapon(NewWeapon, WeaponTag, AttachSocketName, SpawnTransform, AttachToCharacter, bIsCollisionDesabled, InProjectileSocketName);
	}

	return;
}


//const bool UWeaponComponent::IsAnyWeaponEquiped() const
//{
//	return bIsWeaponEquiped || bIsThrowableItemEquiped;
//}
//void UWeaponComponent::CleanThrowableItem()
//{
//	bIsThrowableItemEquiped = false;
//}

//AWeaponBase* UWeaponComponent::EquipWeaponbyClass(TSubclassOf<AWeaponBase> WeaponClass)
//{
//	if (!WeaponClass)
//	{
//		UE_LOGFMT(LogWeaponPlugin, Warning, "{0} - WeaponClass = nullptr", FString(__FUNCTION__));
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

//void UWeaponComponent::OnRep_CurrentWeapon()
//{
//	bIsWeaponEquiped = (CurrentWeapon != nullptr);
//
//#if WITH_EDITOR
//	if (CurrentWeapon)
//	{
//		UE_LOGFMT(LogWeaponPlugin, Log, "{0} - CurrentWeapon: {1}", FString(__FUNCTION__), *CurrentWeapon->GetName());
//	}
//	else
//	{
//		UE_LOGFMT(LogWeaponPlugin, Log, "{0} - CurrentWeapon: nullptr", FString(__FUNCTION__));
//	}
//#endif
//
//}

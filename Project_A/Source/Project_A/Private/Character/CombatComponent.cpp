

#include "Character/CombatComponent.h"
#include "Net/UnrealNetwork.h"
#include "Animation/CharacterAnimInterface.h"
#include "WeaponThrowable.h"
#include "Character/NetPlayerCharacter.h"
//#include "Character/CustomLocomotionComponent.h"

#include "ProjectALog.h"

UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);

}


bool UCombatComponent::IsAttackAvailable() const
{
	if (bIsFighting && BattleState == E_CharacterBattleState::AimThrowable)	{ return true; }
	else if (bIsFighting) { return false; }
	else { return true;	}

	/*return false;*/
}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();
	Initialise();
	
}


void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UCombatComponent, CurrentAttackTags);
	DOREPLIFETIME(UCombatComponent, BattleState);
}

void UCombatComponent::Initialise()
{
	OwnerCharacter = Cast<ANetPlayerCharacter>(GetOwner());
	if (!OwnerCharacter)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - OwnerCharacter = nullptr", FString(__FUNCTION__));
		return;
	}

	OwnerWeaponComponent = OwnerCharacter->GetComponentByClass<UWeaponComponent>();
	if (!OwnerWeaponComponent)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - WeaponComponent = nullptr", FString(__FUNCTION__));        
	}
}

bool UCombatComponent::UpdateAttackState(const FGameplayTag& CurrentAbilityTag, const FGameplayTag& CurrentWeaponTag)
{
	UE_LOGFMT(LogProjectA, Log, "{0} - called ", FString(__FUNCTION__));
	
	// Validations

	if (!IsRunningDedicatedServer())
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - This function should be called only on the server", FString(__FUNCTION__));
		return false;
	}

	if (!CurrentWeaponTag.IsValid())
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - WeaponTag is not valid", FString(__FUNCTION__));
		return false;
	}       
	
	if (!OwnerWeaponComponent)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - OwnerWeaponComponent = nullptr", FString(__FUNCTION__));
		return false;
	}

	//if (bIsFighting 
	//	&& (BattleState != E_CharacterBattleState::AimThrowable))
	//{
	//	UE_LOGFMT(LogProjectA, Log, "{0} - Character is already fighting", FString(__FUNCTION__));
	//	return false;
	//}

	if (!CurrentAttackTags.IsEmpty())
	{
		CurrentAttackTags.Reset();
	}

	// Set Values
	bIsFighting = true; //only for server

	FGameplayTagContainer Temp;
	Temp.AddTag(CurrentAbilityTag);
	Temp.AddTag(CurrentWeaponTag);
	CurrentAttackTags.AppendTags(Temp); // Replicated


	// In this implementation, this block is only used for the previous step(checking bIsFighting and BattleState).

	AWeaponThrowable* ThrowableWeapon = Cast<AWeaponThrowable>(OwnerWeaponComponent->GetAnyActiveEquipedWeapon()); 
	if (ThrowableWeapon)
	{
		if (!ThrowableWeapon->IsAiming())
		{
			UE_LOGFMT(LogProjectA, Log, "{0} - ThrowableWeapon is aiming now", FString(__FUNCTION__));
			BattleState = E_CharacterBattleState::AimThrowable;
			return true;
		}
		else
		{
			UE_LOGFMT(LogProjectA, Log, "{0} - ThrowableWeapon is already aiming", FString(__FUNCTION__));
			BattleState = E_CharacterBattleState::Attack;
			return true;
		}
	}
	else
	{
		UE_LOGFMT(LogProjectA, Log, "{0} - No ThrowableWeapon is equiped", FString(__FUNCTION__));
	}
	
	// Execute weapon attack

	OwnerWeaponComponent->Attack();
	BattleState = E_CharacterBattleState::Attack;

	//Next Step: OnRep_UpdateAttackState
   
	return true;
}

bool UCombatComponent::TryUnequipWeapon(const EInputActionId InputActionType)
{
	UE_LOGFMT(LogProjectA, Log, "{0} - called ", FString(__FUNCTION__));

	bool IsNeedEquipWeapon = false;


	if (!OwnerWeaponComponent)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - OwnerWeaponComponent = nullptr", FString(__FUNCTION__));
		return false;
	}

	switch (InputActionType)
	{
	case EInputActionId::EquipWeapon: {

		if (OwnerWeaponComponent->IsCurrentThrowableItemEquiped())
		{
			OwnerWeaponComponent->UnequipThrowableItem();
			Server_StopAttack();
			//LocomotionComponent->SetCanMove(true); // TODO: LocomotionComponent must be subsribed OnEquiped/Unequiped
		}
		else if (OwnerWeaponComponent->IsCurrentWeaponEquiped())
		{
			OwnerWeaponComponent->UnequipWeapon();            
			Server_StopAttack();
			return false;
		}
		IsNeedEquipWeapon = true;
		break;
	}
	case EInputActionId::EquipThrowableItem: {
		if (OwnerWeaponComponent->IsCurrentWeaponEquiped())
		{
			OwnerWeaponComponent->UnequipWeapon();
			Server_StopAttack();
		}
		else if (OwnerWeaponComponent->IsCurrentThrowableItemEquiped())
		{
			OwnerWeaponComponent->UnequipThrowableItem();           
			Server_StopAttack();
			//LocomotionComponent->SetCanMove(true); // TODO: LocomotionComponent must be subsribed OnEquiped/Unequiped           
			return false;
		}

		IsNeedEquipWeapon = true;
		//LocomotionComponent->SetCanMove(false);  // TODO: LocomotionComponent must be subsribed OnEquiped/Unequiped  
		break;

	default:
		UE_LOGFMT(LogProjectA, Warning, "{0} - Unknown InputActionType", FString(__FUNCTION__));
		return false;
	}
	}

	return IsNeedEquipWeapon;
}

void UCombatComponent::OnAttackAnimationFinished()
{
	UE_LOGFMT(LogProjectA, Log, "{0} - OnAttackAnimationFinished called", FString(__FUNCTION__));
	if (bIsFighting)
	{
		Server_StopAttack();
	}
}

void UCombatComponent::OnSpawnProjectile()
{
	UE_LOGFMT(LogProjectA, Log, "{0} - called", FString(__FUNCTION__));

	if (!OwnerWeaponComponent || !OwnerWeaponComponent->GetCurrentWeapon())
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - OwnerWeaponComponent or CurrentWeapon = nullptr", FString(__FUNCTION__));
		return;
	}

	OwnerWeaponComponent->GetCurrentWeapon()->SpawnProjectile();
}

void UCombatComponent::OnSpawnBombProjectile()
{
	UE_LOGFMT(LogProjectA, Log, "{0} - called", FString(__FUNCTION__));
	if (!OwnerWeaponComponent || !OwnerWeaponComponent->GetCurrentThrowableItem())
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - OwnerWeaponComponent or CurrentWeapon = nullptr", FString(__FUNCTION__));
		return;
	}

	OwnerWeaponComponent->GetCurrentThrowableItem()->SpawnProjectile();
}

void UCombatComponent::StartAnimAttack()
{
	UE_LOGFMT(LogProjectA, Log, "{0} - PlayAttack called", FString(__FUNCTION__));

	if (IsRunningDedicatedServer())
	{
		UE_LOGFMT(LogProjectA, Log, "{0} - Running Dedicated Server, skipping this", FString(__FUNCTION__));
		return;
	}

	if (CurrentAttackTags.IsEmpty())
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - CurrentAttackTags is empty", FString(__FUNCTION__));
		return;
	}

	auto Anim = this->GetAnimInstance();
	if(!Anim)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - AnimInstance = nullptr", FString(__FUNCTION__));
		return;
	}

	ICharacterAnimInterface::Execute_UpdateAttackState(Anim, CurrentAttackTags, bIsFighting, BattleState);
	ICharacterAnimInterface::Execute_SetCanAttack(Anim, bIsFighting);

	CleanCurrentMontage();

	CurrentMontage = FindMontage();

	if (!CurrentMontage)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Montage = nullptr", FString(__FUNCTION__));
		return;
	}   
 
	Anim->Montage_Play(CurrentMontage);
}

UAnimMontage* UCombatComponent::FindMontage()
{
	if (!MontageTable)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - MontageTable = nullptr", FString(__FUNCTION__));
		return nullptr;
	}
	static const FString Context = TEXT("CombatMontageLookup"); 
	
	TArray<FCombatMontageRow*> Rows{};
	MontageTable->GetAllRows(Context, Rows); 
	
	for (const FCombatMontageRow* Row : Rows) 
	{ 
		if (CurrentAttackTags.HasTagExact(Row->AbilityTag) 
			&& CurrentAttackTags.HasAny(Row->WeaponTags)) 
		{ 
			return Row->Montage; 
		} 
	}

	UE_LOGFMT(LogProjectA, Warning, "{0} - No matching montage found in MontageTable", FString(__FUNCTION__));

	return nullptr;
}

UAnimInstance* UCombatComponent::GetAnimInstance() const
{
	if (!OwnerCharacter || !OwnerCharacter->GetMesh())
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - OwnerCharacter or Mesh = nullptr", FString(__FUNCTION__));
		return nullptr;
	}

	auto Anim = OwnerCharacter->GetMesh()->GetAnimInstance();

	if (!Anim)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - AnimInstance = nullptr", FString(__FUNCTION__));
		return nullptr;
	}
	if (!Anim->Implements<UCharacterAnimInterface>())
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - AnimInstance does not implement UCharacterAnimInterface", FString(__FUNCTION__));
		return nullptr;
	}

	return Anim;
}

void UCombatComponent::CleanCurrentMontage()
{
	auto Anim = this->GetAnimInstance();
	if (!Anim)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - AnimInstance = nullptr", FString(__FUNCTION__));
		return;
	}

	if (Anim->Montage_IsPlaying(CurrentMontage))
	{
		Anim->Montage_Stop(0.2f, CurrentMontage);
		CurrentMontage = nullptr;
		UE_LOGFMT(LogProjectA, Log, "{0} - CurrentMontage stopped and cleaned", FString(__FUNCTION__));
	}
	else
	{
		UE_LOGFMT(LogProjectA, Log, "{0} - No montage is playing, nothing to clean", FString(__FUNCTION__));
	}
			
}

void UCombatComponent::Server_StopAttack_Implementation()
{
	UE_LOGFMT(LogProjectA, Log, "{0} - Server_StopAttack called", FString(__FUNCTION__));

	if (!bIsFighting)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Character is not fighting", FString(__FUNCTION__));
		return;
	}
		
	OwnerWeaponComponent->StopAttack();
	BattleState = E_CharacterBattleState::Normal;
	CurrentAttackTags.Reset();
	bIsFighting = false;
	Multicast_StopAttack();
}

void UCombatComponent::Multicast_StopAttack_Implementation()
{
	
	if (IsRunningDedicatedServer())
	{
		UE_LOGFMT(LogProjectA, Log, "{0} - Running Dedicated Server, skipping this", FString(__FUNCTION__));
		return;
	}

	bIsFighting = false;

	auto Anim = this->GetAnimInstance();
	CleanCurrentMontage();
	ICharacterAnimInterface::Execute_SetCanAttack(Anim, bIsFighting);
	ICharacterAnimInterface::Execute_UpdateAttackState(Anim, CurrentAttackTags, bIsFighting, BattleState); // TODO: check if needed
}

void UCombatComponent::OnRep_UpdateAttackState()
{
	UE_LOGFMT(LogProjectA, Log, "{0} - OnRep_UpdateAttackState called", FString(__FUNCTION__));

	if (CurrentAttackTags.IsEmpty())
	{
		UE_LOGFMT(LogProjectA, Log, "{0} - CurrentAttackTags is empty", FString(__FUNCTION__));
		return;
	}

	bIsFighting = true; //for clients
	StartAnimAttack();
}




//////// Logging //////
//#if WITH_EDITOR
//
//
//FString OwnerWeaponCompName = OwnerWeaponComponent ? OwnerWeaponComponent->GetName() : TEXT("null");
//AWeaponBase* CurrentWeaponPtr = OwnerWeaponComponent ? OwnerWeaponComponent->GetCurrentWeapon() : nullptr;
//FString CurrentWeaponName = CurrentWeaponPtr ? CurrentWeaponPtr->GetName() : TEXT("null");
//FString AbilityTagValid = CurrentAbilityTag.IsValid() ? TEXT("true") : TEXT("false");
//FString WeaponTagValid = CurrentWeaponTag.IsValid() ? TEXT("true") : TEXT("false");
//
//UE_LOGFMT(LogProjectA, Log, "{0} - called. OwnerWeaponComp={1}, CurrentWeapon={2}, AbilityTag.Valid={3}, WeaponTag.Valid={4}",
//	FString(__FUNCTION__), OwnerWeaponCompName, CurrentWeaponName, AbilityTagValid, WeaponTagValid);
//#endif // WITH_EDITOR
//////// Logging //////
/// E_CharacterBattleState UCombatComponent::GetBattleState()
//{
//	return BattleState;
//}

//void UCombatComponent::UnequipAnyWeapon(const EInputActionId InputActionType)
//{
//    switch (InputActionType)
//    {
//    case EInputActionId::EquipWeapon:
//        WeaponComponent->UnequipWeapon();
//        break;
//    case EInputActionId::EquipThrowableItem:
//        WeaponComponent->UnequipThrowableItem();
//        break;
//    default:
//        UE_LOGFMT(LogProjectA, Warning, "{0} - Unknown InputActionType", FString(__FUNCTION__));
//        break;
//    }
//}
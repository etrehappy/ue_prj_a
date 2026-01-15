#include "Character/NetPlayerCharacter.h"

#include "AbilityComponent.h"
#include "Animation/CharacterAnimInterface.h"
#include "Camera/CameraComponent.h"
#include "Character/CustomInputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameplayTagContainer.h"
#include "Net/UnrealNetwork.h"

#include "ProjectALog.h"

ANetPlayerCharacter::ANetPlayerCharacter()
{	
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;
	SetReplicateMovement(true);

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	LocomotionComponent = CreateDefaultSubobject<UCustomLocomotionComponent>(TEXT("LocomotionComponent"));
	LocomotionComponent->SetComponentTickEnabled(false);

	CustomInputComponent = CreateDefaultSubobject<UCustomInputComponent>(TEXT("CustomInputComponent"));
	CustomInputComponent->SetComponentTickEnabled(true);

	WeaponComponent = CreateDefaultSubobject<UWeaponComponent>(TEXT("WeaponComponent"));
	WeaponComponent->SetComponentTickEnabled(false);

	AbilityComponent = CreateDefaultSubobject<UAbilityComponent>(TEXT("AbilityComponent"));
	AbilityComponent->SetComponentTickEnabled(false);

	CombatComponent = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
	CombatComponent->SetComponentTickEnabled(false);
}

void ANetPlayerCharacter::Destroyed()
{
	Super::Destroyed();

	if (!WeaponComponent)
	{
		UE_LOGFMT(LogProjectA, Log, "{0} - WeaponComponent = nullptr", FString(__FUNCTION__));			
	}
	else if(WeaponComponent->IsCurrentWeaponEquiped())
	{
		WeaponComponent->UnequipWeapon();
	}
	else if (WeaponComponent->IsCurrentThrowableItemEquiped())
	{
		WeaponComponent->UnequipThrowableItem();
	}

}

void ANetPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	AbilityComponent->OnAbilityActivated.AddDynamic(this, &ANetPlayerCharacter::HandleAbilityActivated);
}

void ANetPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ANetPlayerCharacter::EquipWeapon(EInputActionId InputActionType)
{
	if (CombatComponent->TryUnequipWeapon(InputActionType) == false)
	{
		UE_LOGFMT(LogProjectA, Log, "{0} - No need to equip weapon", FString(__FUNCTION__));
		return;
	}

	OnEquipWeapon(InputActionType);
}

void ANetPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {

		LocomotionComponent->BindActions(EnhancedInputComponent);
		CustomInputComponent->BindActions(EnhancedInputComponent);
	}
	else
	{
		UE_LOGFMT(LogProjectA, Error, "{0} -  Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file.", *GetNameSafe(this));
	}

}

void ANetPlayerCharacter::HandleAbilityActivated(FGameplayTag AbilityTag)
{
	UE_LOGFMT(LogProjectA, Log, "{0} - called ", FString(__FUNCTION__));

	auto Anim = this->GetMesh()->GetAnimInstance();

	if (!Anim)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - AnimInstance = nullptr", FString(__FUNCTION__));
		return;
	}

	if (!Anim->Implements<UCharacterAnimInterface>())
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - AnimInstance does not implement UCharacterAnimInterface", FString(__FUNCTION__));
		return;
	}

	if (!WeaponComponent)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - WeaponComponent = nullptr", FString(__FUNCTION__));
		return;
	}

	AWeaponBase* ActiveWeapon = WeaponComponent->GetAnyActiveEquipedWeapon();
	if (!ActiveWeapon)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - No weapon is equipped", FString(__FUNCTION__));
		return;
	}

	if (!CombatComponent->UpdateAttackState(AbilityTag, ActiveWeapon->GetWeaponTag()))
	{
		UE_LOGFMT(LogProjectA, Log, "{0} - Failed to update CurrentAttackTags", FString(__FUNCTION__));
		return;
	}
}




//void ANetPlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
//{
//	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
//}
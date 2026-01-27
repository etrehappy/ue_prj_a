#include "Character/NetPlayerCharacter.h"

#include "AbilityComponent.h"
#include "Animation/CharacterAnimInterface.h"
#include "Camera/CameraComponent.h"
#include "Character/CustomInputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameplayTagContainer.h"
#include "Net/UnrealNetwork.h"
#include "HealthComponent.h"
#include "InventoryComponent.h"
#include "ItemPickup.h"

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

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
	HealthComponent->SetComponentTickEnabled(false);

	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
	InventoryComponent->SetComponentTickEnabled(false);
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

void ANetPlayerCharacter::Interact()
{
	Server_TryInteract();
}

void ANetPlayerCharacter::UpdatedCurrentPickUpItem(AItemPickup* Item)
{	
	IPickUpInterface::Execute_ShowPopup(this, Item);
	Server_UpdatedCurrentPickUpItem(Item);		
}

void ANetPlayerCharacter::CleanCurrentPickUpItem()
{
	IPickUpInterface::Execute_HidePopup(this);
	Server_CleanCurrentPickUpItem();
}

void ANetPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	AbilityComponent->OnAbilityActivated.AddDynamic(this, &ANetPlayerCharacter::HandleAbilityActivated);
	HealthComponent->OnDeath.AddDynamic(this, &ANetPlayerCharacter::OnDead);
	HealthComponent->OnIncreaseHealth.AddDynamic(this, &ANetPlayerCharacter::OnIncreaseHealth);
	HealthComponent->OnDecreaseHealth.AddDynamic(this, &ANetPlayerCharacter::OnDecreaseHealth);	

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

void ANetPlayerCharacter::Server_CleanCurrentPickUpItem_Implementation()
{
	CurrentInteractable = nullptr;
}

void ANetPlayerCharacter::OnDead()
{
	UE_LOGFMT(LogProjectA, Log, "{0} - called ", FString(__FUNCTION__));
	Multicast_PlayDeathFX();
}

void ANetPlayerCharacter::OnIncreaseHealth(float HealAmount)
{
	UE_LOGFMT(LogProjectA, Log, "{0} - called ", FString(__FUNCTION__));
	Multicast_PlayHealFX(HealAmount);
}

void ANetPlayerCharacter::OnDecreaseHealth(float DamageAmount)
{
	UE_LOGFMT(LogProjectA, Log, "{0} - called ", FString(__FUNCTION__));
	Multicast_PlayDamageFX(DamageAmount);
}

void ANetPlayerCharacter::Multicast_PlayHealFX_Implementation(float HealAmount)
{
	UE_LOGFMT(LogProjectA, Log, "{0} - called ", FString(__FUNCTION__));
	if(IsRunningDedicatedServer())
	{
		UE_LOGFMT(LogProjectA, Log, "{0} - called on a dedicated server, skipping ", FString(__FUNCTION__));
		return;
	}

	OnPlayHealFX(HealAmount);
}		

void ANetPlayerCharacter::Multicast_PlayDamageFX_Implementation(float DamageAmount)
{
	UE_LOGFMT(LogProjectA, Log, "{0} - called ", FString(__FUNCTION__));
	if (IsRunningDedicatedServer())
	{
		UE_LOGFMT(LogProjectA, Log, "{0} - called on a dedicated server, skipping ", FString(__FUNCTION__));
		return;
	}

	OnPlayDamageFX(DamageAmount);
}

void ANetPlayerCharacter::Multicast_PlayDeathFX_Implementation()
{
	UE_LOGFMT(LogProjectA, Log, "{0} - called ", FString(__FUNCTION__));
	if (IsRunningDedicatedServer())
	{
		UE_LOGFMT(LogProjectA, Log, "{0} - called on a dedicated server, skipping ", FString(__FUNCTION__));
		return;
	}

	OnPlayDeathFX();
}

void ANetPlayerCharacter::Server_OnDeathFxFinished_Implementation()
{
	UE_LOGFMT(LogProjectA, Log, "{0} - called ", FString(__FUNCTION__));
	
	AController* PController = GetController();
	if(!PController)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - PlayerController = nullptr", FString(__FUNCTION__));
		return;
	}

	
	PController->UnPossess();


	AGameModeBase* GameMode = GetWorld() ? GetWorld()->GetAuthGameMode() : nullptr;
	if (GameMode)
	{		
		Destroy();
				
		GameMode->RestartPlayer(PController);
		UE_LOGFMT(LogProjectA, Log, "{0} - RestartPlayer called", FString(__FUNCTION__));
	}
	else
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - GameMode not found, cannot restart player", FString(__FUNCTION__));
	}
}

void ANetPlayerCharacter::PickUpItem_Implementation(AItemPickup* Item)
{
	UE_LOGFMT(LogProjectA, Log, "{0} - called ", FString(__FUNCTION__));
	

	InventoryComponent->AddToInventory(Item);
}

void ANetPlayerCharacter::Server_UpdatedCurrentPickUpItem_Implementation(AItemPickup* Item)
{
	if (!Item)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Item ie empty", FString(__FUNCTION__));
	}

	const float MaxPickupDist = 300.f;
	const float DistSq = FVector::DistSquared(Item->GetActorLocation(), GetActorLocation());
	if (DistSq > FMath::Square(MaxPickupDist))
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - item too far", FString(__FUNCTION__));
		return;
	}

	CurrentInteractable = Item;
}

void ANetPlayerCharacter::Server_TryInteract_Implementation()
{
	UE_LOGFMT(LogProjectA, Log, "{0} - called", FString(__FUNCTION__));

	/* Temporary solution  */

	if (auto Item = Cast<AItemPickup>(CurrentInteractable))
	{
		Item->PickUp(this);
	}
	else
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Item is not AItemPickup", FString(__FUNCTION__));
	}
}




//void ANetPlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
//{
//	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
//}
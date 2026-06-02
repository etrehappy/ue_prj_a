#include "Character/NetPlayerCharacter.h"

#include "Character/NetPlayerCharacter.h"

#include "AbilityComponent.h"
#include "Animation/CharacterAnimInterface.h"
#include "Camera/CameraComponent.h"
#include "Character/CustomInputComponent.h"
#include "Character/CustomPlayerController.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameplayTagContainer.h"
#include "GeneralHud.h"
#include "HealthComponent.h"
#include "InteractionComponent.h"
#include "InventoryComponent.h"
#include "InventoryItem.h"
#include "ItemPickup.h"
#include "Net/UnrealNetwork.h"
#include "StatusEffect/StatusEffectsComponent.h"
#include "QuestLogComponent.h"
#include "Core/WorldGameMode.h"
#include "PlayerDialogueComponent.h"

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

	InteractionComponent = CreateDefaultSubobject<UInteractionComponent>(TEXT("InteractionComponent"));
	InteractionComponent->SetComponentTickEnabled(true);

	StatusEffectComponent = CreateDefaultSubobject<UStatusEffectComponent>(TEXT("StatusEffectComponent"));
	StatusEffectComponent->SetComponentTickEnabled(false);

	QuestLogComponent = CreateDefaultSubobject<UQuestLogComponent>(TEXT("QuestLogComponent"));
	QuestLogComponent->SetComponentTickEnabled(false);

	PlayerDialogueComponent = CreateDefaultSubobject<UPlayerDialogueComponent>(TEXT("PlayerDialogueComponent"));
	PlayerDialogueComponent->SetComponentTickEnabled(false);

	if (auto* Capsule = GetCapsuleComponent())
	{
		Capsule->SetGenerateOverlapEvents(true);
		Capsule->OnComponentBeginOverlap.AddDynamic(this, &ANetPlayerCharacter::OtherBeginOverlap);
		Capsule->OnComponentEndOverlap.AddDynamic(this, &ANetPlayerCharacter::OtherEndOverlap);
	}
	else
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - No collision component found on owner {1}", FString(__FUNCTION__), *Owner->GetName());
	}
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
	// It is called by "E" on client

	// If  an object has interaction actions, we want to open interaction menu.
	if (InteractionComponent)
	{
		InteractionComponent->TryInteract();
	}

	// If an object hasn't interaction actions, but it has dialogue, we want to open dialogue widget. 
	if (PlayerDialogueComponent)
	{
		PlayerDialogueComponent->RequestDialogueSnapshot();
	}
}

void ANetPlayerCharacter::ToggleInventory()
{
	if (IsRunningDedicatedServer())
	{
		UE_LOGFMT(LogProjectA, Log, "{0} - Running on dedicated server, skip", FString(__FUNCTION__));
		return;
	}

	AController* PController = GetController();
	if (!PController)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - PController = nullptr", FString(__FUNCTION__));
		return;
	}
	ACustomPlayerController* PlayerController = Cast<ACustomPlayerController>(PController);
	if (!PlayerController)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - PlayerController is nullptr", FString(__FUNCTION__));
		return;
	}

	AGeneralHud* HUD = Cast<AGeneralHud>(PlayerController->GetHUD());
	if (!HUD)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - HUD is nullptr", FString(__FUNCTION__));
		return;
	}

	HUD->ToggleInventory();
}


void ANetPlayerCharacter::ToggleMainMenu()
{
	if (IsRunningDedicatedServer())
	{
		UE_LOGFMT(LogProjectA, Log, "{0} - Running on dedicated server, skip", FString(__FUNCTION__));
		return;
	}

	AController* PController = GetController();
	if (!PController)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - PController = nullptr", FString(__FUNCTION__));
		return;
	}
	ACustomPlayerController* PlayerController = Cast<ACustomPlayerController>(PController);
	if (!PlayerController)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - PlayerController is nullptr", FString(__FUNCTION__));
		return;
	}

	AGeneralHud* HUD = Cast<AGeneralHud>(PlayerController->GetHUD());
	if (!HUD)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - HUD is nullptr", FString(__FUNCTION__));
		return;
	}

	HUD->ToggleMainMenu();
}

void ANetPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (AbilityComponent)
	{
		AbilityComponent->OnAbilityActivated.AddDynamic(this, &ANetPlayerCharacter::HandleAbilityActivated);
	}	

	if (HealthComponent)
	{
		HealthComponent->OnDeath.AddDynamic(this, &ANetPlayerCharacter::OnDead);
		HealthComponent->OnIncreaseHealth.AddDynamic(this, &ANetPlayerCharacter::OnIncreaseHealth);
		HealthComponent->OnDecreaseHealth.AddDynamic(this, &ANetPlayerCharacter::OnDecreaseHealth);
		HealthComponent->OnHealthChanged.AddDynamic(this, &ANetPlayerCharacter::HandleHealthChanged);

		HandleHealthChanged(HealthComponent->GetCurrentHealth(), HealthComponent->GetMaxHealth());
	}	


	if (InteractionComponent)
	{
		InteractionComponent->OnFocusChanged.AddUObject(this, &ANetPlayerCharacter::HandleFocusChanged);
		InteractionComponent->OnInteractionReceived.AddUObject(this, &ANetPlayerCharacter::HandleInteractionReceived);
	}

	if (PlayerDialogueComponent)
	{
		PlayerDialogueComponent->OnDialogueNodeReceived.AddUObject(this, &ANetPlayerCharacter::HandleDialogueNodeReceived);
		PlayerDialogueComponent->OnDialogueClosed.AddUObject(this, &ANetPlayerCharacter::HandleDialogueClosed);
	}

	RestoreCollision();	
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

void ANetPlayerCharacter::HandleFocusChanged(AActor* NewFocusedActor)
{
	ACustomPlayerController* PC = Cast<ACustomPlayerController>(GetController());
	if (PC)
	{
		PC->OnFocusChanged(NewFocusedActor);
	}
}

void ANetPlayerCharacter::OtherBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{	
	if (IsRunningDedicatedServer())
	{
		InteractionComponent->OtherBeginOverlap(OtherActor);
	}
	
}

void ANetPlayerCharacter::OtherEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (IsRunningDedicatedServer())
	{
		InteractionComponent->OtherEndOverlap(OtherActor);
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

void ANetPlayerCharacter::OnDead()
{
	UE_LOGFMT(LogProjectA, Log, "{0} - called ", FString(__FUNCTION__));

	DisableCharacterControl();
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

void ANetPlayerCharacter::HandleHealthChanged(float CurrentHealthValue, float MaxHealthValue)
{
	if (!IsLocallyControlled())
	{
		return;
	}

	ACustomPlayerController* PlayerController = Cast<ACustomPlayerController>(GetController());
	if (!PlayerController)
	{
		return;
	}

	const bool bIsDead = CurrentHealthValue <= 0.f;
	PlayerController->SetDeathMenuVisible(bIsDead);
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

void ANetPlayerCharacter::DisableCharacterControl()
{
	if (UCharacterMovementComponent* MovementComp = GetCharacterMovement())
	{
		MovementComp->StopMovementImmediately();
		MovementComp->DisableMovement();
	}

	if (AController* CharacterController = GetController())
	{
		CharacterController->SetIgnoreMoveInput(true);
		CharacterController->SetIgnoreLookInput(true);
	}

	if (UCapsuleComponent* Capsule = GetCapsuleComponent())
	{
		Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void ANetPlayerCharacter::RestoreCollision()
{
	UCapsuleComponent* Capsule = GetCapsuleComponent();

	if (!Capsule)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - CapsuleComponent = nullptr", FString(__FUNCTION__));
		return;	
	}

	Capsule->SetCollisionProfileName(TEXT("Pawn"));
}

void ANetPlayerCharacter::Multicast_PlayDeathFX_Implementation()
{
	UE_LOGFMT(LogProjectA, Log, "{0} - called ", FString(__FUNCTION__));
	if (IsRunningDedicatedServer())
	{
		UE_LOGFMT(LogProjectA, Log, "{0} - called on a dedicated server, skipping ", FString(__FUNCTION__));
		return;
	}

	DisableCharacterControl();
	OnPlayDeathFX();
}

void ANetPlayerCharacter::Server_OnDeathFxFinished_Implementation()
{
	UE_LOGFMT(LogProjectA, Log, "{0} - called ", FString(__FUNCTION__));
	
}

bool ANetPlayerCharacter::PickUpItem_Implementation(AItemPickup* Item)
{
	UE_LOGFMT(LogProjectA, Log, "{0} - called ", FString(__FUNCTION__));
	bool bWasPickedUp = false;

	if (!IsValid(InventoryComponent))
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - InventoryComponent = nullptr", FString(__FUNCTION__));
		return bWasPickedUp;
	}

	UInventoryItem* InventoryItem = NewObject<UInventoryItem>();
	InventoryItem->Definition = Item->GetItemDefinition();
	InventoryItem->StackCount = 1;
	bWasPickedUp = InventoryComponent->AddToInventory(InventoryItem);

	if (!bWasPickedUp)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Failed to add item to inventory", FString(__FUNCTION__));
	}

	return bWasPickedUp;
}


bool ANetPlayerCharacter::CanInteract(APawn* RequestInteractor) const
{
	bool bCanInteractWithRequester = RequestInteractor && (RequestInteractor != this);

	return bCanInteractWithRequester;
}

//void ANetPlayerCharacter::Interact(APawn* Interactor)
//{	
//}


void ANetPlayerCharacter::BuildInteractionActions(APawn* Interactor, TArray<FInteractionActionType>& OutActions) const
{
	OutActions.Reset();

	if (!CanInteract(Interactor))
	{
		return;
	}

	for (const TObjectPtr<UInteractionActionDefinition>& ActionDef : InteractionActions)
	{
		if (!ActionDef || !ActionDef->ActionTag.IsValid())
		{
			continue;
		}

		FInteractionActionType ActionView{};
		ActionView.bIsEnabled = ActionDef->bEnabledByDefault; 
		ActionView.Definition = ActionDef;

		OutActions.Add(ActionView);
	}
}

bool ANetPlayerCharacter::ExecuteInteractionAction(APawn* Interactor, FGameplayTag ActionTag)
{
	if (!CanInteract(Interactor) || !ActionTag.IsValid())
	{
		return false;
	}

	// 1. Check if the action tag matches any of the defined interaction actions. 
	const UInteractionActionDefinition* MatchedActionDef = nullptr;

	for (const TObjectPtr<UInteractionActionDefinition>& ActionDef : InteractionActions)
	{
		if (!ActionDef || !ActionDef->ActionTag.IsValid())
		{
			continue;
		}

		if (ActionDef->ActionTag.MatchesTagExact(ActionTag))
		{
			MatchedActionDef = ActionDef;
			break;
		}
	}

	if (!MatchedActionDef || !MatchedActionDef->bEnabledByDefault)
	{
		return false;
	}

	return BP_ExecuteInteractionAction(Interactor, ActionTag);
}

void ANetPlayerCharacter::HandleInteractionReceived(AActor* TargetActor, const TArray<FInteractionActionType>& Actions)
{
	if (IsRunningDedicatedServer())
	{
		UE_LOGFMT(LogProjectA, Log, "{0} - called on a dedicated server, skipping ", FString(__FUNCTION__));
		return;
	}

	if (!TargetActor || Actions.Num() == 0)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Invalid target actor or empty actions array", FString(__FUNCTION__));
		return;
	}

	ACustomPlayerController* PlayerController = Cast<ACustomPlayerController>(GetController());
	if (!PlayerController)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - PlayerController is nullptr", FString(__FUNCTION__));
		return;
	}

	AGeneralHud* HUD = Cast<AGeneralHud>(PlayerController->GetHUD());
	if (!HUD)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - HUD is nullptr", FString(__FUNCTION__));
		return;
	}

	HUD->ShowInteractionMenu(TargetActor, Actions);
}

void ANetPlayerCharacter::HandleDialogueNodeReceived(const FDialogueNodeRuntime& Node)
{
	if (IsRunningDedicatedServer())
	{
		return;
	}

	ACustomPlayerController* PlayerController = Cast<ACustomPlayerController>(GetController());
	if (!PlayerController)
	{
		return;
	}

	AGeneralHud* HUD = Cast<AGeneralHud>(PlayerController->GetHUD());
	if (!HUD)
	{
		return;
	}

	HUD->ShowDialogueNode(Node);
}

void ANetPlayerCharacter::HandleDialogueClosed()
{
	if (IsRunningDedicatedServer())
	{
		return;
	}

	ACustomPlayerController* PlayerController = Cast<ACustomPlayerController>(GetController());
	if (!PlayerController)
	{
		return;
	}

	AGeneralHud* HUD = Cast<AGeneralHud>(PlayerController->GetHUD());
	if (!HUD)
	{
		return;
	}

	HUD->HideDialogue();
}
#include "Character/CustomInputComponent.h"
#include "Character/CombatComponent.h"
#include "Character/NetPlayerCharacter.h"
#include "AbilityComponent.h"

#include "ProjectALog.h"


UCustomInputComponent::UCustomInputComponent()
{

	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
	// ...
}

void UCustomInputComponent::BeginPlay()
{
	Super::BeginPlay();

	Initialise();
	
}


void UCustomInputComponent::Initialise()
{
	Owner = Cast<ANetPlayerCharacter>(GetOwner());    

	if (!Owner)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Owner = nullptr", FString(__FUNCTION__));
		return;
	}   
}

// Called every frame
void UCustomInputComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}


void UCustomInputComponent::BindActions(UEnhancedInputComponent* EIC)
{
	if (IsRunningDedicatedServer())
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Running Dedicated Server, skipping this", FString(__FUNCTION__));
		return;
	}

	if (InputActions.EquipWeapon)
	{
		const EInputActionId InputActionType = EInputActionId::EquipWeapon;

		EIC->BindAction(InputActions.EquipWeapon, ETriggerEvent::Completed, this, &UCustomInputComponent::EquipWeapon);
	}

	if (InputActions.EquipThrowableItem)
	{
		EIC->BindAction(InputActions.EquipThrowableItem, ETriggerEvent::Completed, this, &UCustomInputComponent::EquipThrowableItem);
	}

	if (InputActions.SimpleAttack)
	{
		EIC->BindAction(InputActions.SimpleAttack, ETriggerEvent::Completed, this, &UCustomInputComponent::HandleAbility);
	}


	if (InputActions.ThrowThrowableItem)
	{
		EIC->BindAction(InputActions.ThrowThrowableItem, ETriggerEvent::Completed, this, &UCustomInputComponent::HandleAbility);
	}

	if (InputActions.Interaction)
	{
		EIC->BindAction(InputActions.Interaction, ETriggerEvent::Completed, this, &UCustomInputComponent::Interaction);
	}

	if (InputActions.ToggleInventory)
	{
		EIC->BindAction(InputActions.ToggleInventory, ETriggerEvent::Completed, this, &UCustomInputComponent::ToggleInventory);
	}

	if (InputActions.ToggleMainMenu)
	{
		EIC->BindAction(InputActions.ToggleMainMenu, ETriggerEvent::Completed, this, &UCustomInputComponent::ToggleMainMenu);
	}

}

void UCustomInputComponent::EquipWeapon(const FInputActionValue& Value)
{
	Owner->EquipWeapon(EInputActionId::EquipWeapon);
}

void UCustomInputComponent::EquipThrowableItem(const FInputActionInstance& Instance)
{
	Owner->EquipWeapon(EInputActionId::EquipThrowableItem);
	HandleAbility(Instance);
}

void UCustomInputComponent::HandleAbility(const FInputActionInstance& Instance)
{
	const auto CombatComponent = Owner->GetComponentByClass<UCombatComponent>();
	if(!CombatComponent)
	{   
		UE_LOGFMT(LogProjectA, Warning, "{0} - CombatComponent = nullptr", FString(__FUNCTION__));
		return;
	}

	if (!CombatComponent->IsAttackAvailable()) // client side check
	{
		UE_LOGFMT(LogProjectA, Log, "{0} - IsAttackAvailable = false, skipping input", FString(__FUNCTION__));
		return;
	}

	const auto AbilityComponent = Owner->GetComponentByClass<UAbilityComponent>();
	if(!AbilityComponent)
	{   
		UE_LOGFMT(LogProjectA, Warning, "{0} - AbilityComponent = nullptr", FString(__FUNCTION__));
		return;
	}
	ETriggerEvent InputEvent = Instance.GetTriggerEvent();
	FName InputName = Instance.GetSourceAction()->GetFName();

	AbilityComponent->HandleInputAction(InputName, InputEvent);
}

void UCustomInputComponent::Interaction(const FInputActionValue& Value)
{
	Owner->Interact();
}

void UCustomInputComponent::ToggleInventory(const FInputActionValue& Value)
{
	if (!Owner)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Owner = nullptr", FString(__FUNCTION__));
		return;
	}

	Owner->ToggleInventory();
}

void UCustomInputComponent::ToggleMainMenu(const FInputActionValue& Value)
{
	if (!Owner)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Owner = nullptr", FString(__FUNCTION__));
		return;
	}

	Owner->ToggleMainMenu();
}




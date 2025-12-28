


#include "Character/BattleComponent.h"
#include "Character/NetPlayerCharacter.h"
#include "ProjectALog.h"

// Sets default values for this component's properties
UBattleComponent::UBattleComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void UBattleComponent::BeginPlay()
{
	Super::BeginPlay();

    Initialize();
	
}


void UBattleComponent::Initialize()
{
    Owner = Cast<ANetPlayerCharacter>(GetOwner());    

    if (!Owner)
    {
        UE_LOGFMT(LogProjectA, Warning, "{0} — Owner = nullptr", FString(__FUNCTION__));
        return;
    }   
}


// Called every frame
void UBattleComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}


void UBattleComponent::BindActions(UEnhancedInputComponent* EIC)
{
    if (IsRunningDedicatedServer())
    {
        UE_LOGFMT(LogProjectA, Warning, "{0} — Running Dedicated Server, skipping this", FString(__FUNCTION__));
        return;
    }

    if (InputActionEquipWeapon)
    {
        EIC->BindAction(InputActionEquipWeapon, ETriggerEvent::Completed, this, &UBattleComponent::EquipWeapon);
    }

}

void UBattleComponent::EquipWeapon(const FInputActionValue& Value)
{
    Owner->OnEquipWeapon();
}




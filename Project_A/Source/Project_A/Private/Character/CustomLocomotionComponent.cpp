#include "Character/CustomLocomotionComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"

DEFINE_LOG_CATEGORY(LogMyGame);

// Sets default values for this component's properties
UCustomLocomotionComponent::UCustomLocomotionComponent()
    :MoveInputScale{1.f}
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UCustomLocomotionComponent::BeginPlay()
{
	Super::BeginPlay();

    Initialize();

	// ...
	
}


// Called every frame
void UCustomLocomotionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}



ACharacter* UCustomLocomotionComponent::GetOwnerCharacter() const
{
    return Cast<ACharacter>(GetOwner());
}

ACustomPlayerController* UCustomLocomotionComponent::GetCustomPlayerController() const
{
    if (const ACharacter* Character = GetOwnerCharacter())
    {
        return Cast<ACustomPlayerController>(Character->GetController());
    }
    return nullptr;
}

void UCustomLocomotionComponent::Initialize()
{   
    Owner = GetOwner();
    CustomPC = GetCustomPlayerController();
    if (!Owner || !CustomPC)
    {
        UE_LOGFMT(LogTemp, Warning, "{0} Owner or CustomPC = nullptr", FString(__FUNCTION__));
        return;
    }

    for (const FInputMappingContextWithPriority& Item : InputMappingContext)
    {
        if (Item.MappingContext)
        {
            CustomPC->AddInputMappingContext(Item.MappingContext, Item.Priority);            
        }
    }

    if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(CustomPC->InputComponent))
    {
        if (InputActionMove)
        {
            EIC->BindAction(InputActionMove, ETriggerEvent::Triggered, this, &UCustomLocomotionComponent::HandleMove);
        }
        if (InputActionLook)
        {
            EIC->BindAction(InputActionLook, ETriggerEvent::Triggered, this, &UCustomLocomotionComponent::HandleLook);
        }
    }
}

void UCustomLocomotionComponent::HandleMove(const FInputActionValue& Value)
{
     ACharacter* const OwnerCharacter = GetOwnerCharacter();
     const FVector2D MoveInput = Value.Get<FVector2D>();

     if (!IsVectorInputValid(OwnerCharacter, MoveInput))
     {
         return;
     }

    OwnerCharacter->AddMovementInput(OwnerCharacter->GetActorForwardVector(), MoveInput.Y * MoveInputScale);
    OwnerCharacter->AddMovementInput(OwnerCharacter->GetActorRightVector(), MoveInput.X * MoveInputScale);
}

void UCustomLocomotionComponent::HandleLook(const FInputActionValue& Value)
{    
    ACharacter* const OwnerCharacter = GetOwnerCharacter();
    const FVector2D LookInput = Value.Get<FVector2D>();

    if (!IsVectorInputValid(OwnerCharacter, LookInput))
    {
        return;
    }

    if (OwnerCharacter->GetController() != nullptr)
    {
        OwnerCharacter->AddControllerYawInput(LookInput.X);
        OwnerCharacter->AddControllerPitchInput(LookInput.Y);
    }
}



bool UCustomLocomotionComponent::IsVectorInputValid(ACharacter* const OwnerCharacter, const FVector2D& Input) const
{
    if (!OwnerCharacter)
    {
        UE_LOGFMT(LogTemp, Warning, "{0} OwnerCharacter = nullptr", FString(__FUNCTION__));
        return false;
    }
    
    if (Input.IsNearlyZero())
    {
        return false;
    }

    return true;
}




///////////////////////////////////////////////////////////////////////////////
//AutoTests

const TArray<const UInputAction*> UCustomLocomotionComponent::AutoTestGetInputActions() const
{
    TArray<const UInputAction*> Actions{};
    Actions.Add(InputActionMove);
    Actions.Add(InputActionLook);

    return Actions;
}

const TArray<FInputMappingContextWithPriority> UCustomLocomotionComponent::AutoTestGetInputMappingContext() const
{
    return InputMappingContext;
}

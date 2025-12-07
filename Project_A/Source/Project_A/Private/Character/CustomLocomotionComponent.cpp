#include "Character/CustomLocomotionComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "CommonSettings.h"
#include "GameFramework/CharacterMovementComponent.h" 

DEFINE_LOG_CATEGORY(LogMyGame);


////////////////////////////////////////////////////////////////////////////////

// Sets default values for this component's properties
UCustomLocomotionComponent::UCustomLocomotionComponent()
    :MoveInputScale{1.f}, BackwordSpeed{200.f}, RunSpeed{350.f}, SprintSpeed{500.f}
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
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

    CurrentSpeed2D = Owner->GetVelocity().Length();
}

void UCustomLocomotionComponent::Initialize()
{   
    Owner = Cast<ABaseCharacter>(GetOwner() );
    CustomPC = GetCustomPlayerController();
    CharacterMovementComponent = Owner->GetCharacterMovement();

    if (!Owner || !CustomPC)
    {
        UE_LOGFMT(LogTemp, Warning, "{0} Owner or CustomPC = nullptr", FString(__FUNCTION__));
        return;
    }

    for (const FInputMappingContextWithPriority& Item : Inputs.InputMappingContext)
    {
        if (Item.MappingContext)
        {
            CustomPC->AddInputMappingContext(Item.MappingContext, Item.Priority);            
        }
    }

    if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(CustomPC->InputComponent))
    {
        this->BindActions(EIC);
    }
    
    if (CharacterMovementComponent)
    {
        Owner->MovementModeChangedDelegate.AddDynamic(this, &UCustomLocomotionComponent::OnMovementModeChanged);
    }
    else
    {
        UE_LOGFMT(LogTemp, Warning, "{0} CharacterMovementComponent = nullptr", FString(__FUNCTION__));
    }
   
}

ACustomPlayerController* UCustomLocomotionComponent::GetCustomPlayerController() const
{
    if (Owner)
    {
        return Cast<ACustomPlayerController>(Owner->GetController());
    }
    return nullptr;
}

bool UCustomLocomotionComponent::IsVectorInputValid(ACharacter* const OwnerCharacter, const FVector2D& Input) const
{
    if (!Owner)
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

void UCustomLocomotionComponent::BindActions(UEnhancedInputComponent* EIC)
{
    if (Inputs.InputActionMove)
    {
        EIC->BindAction(Inputs.InputActionMove, ETriggerEvent::Triggered, this, &UCustomLocomotionComponent::StartMove);
    }
    if (Inputs.InputActionLook)
    {
        EIC->BindAction(Inputs.InputActionLook, ETriggerEvent::Triggered, this, &UCustomLocomotionComponent::HandleLook);
    }

    if (Inputs.InputActionJump)
    {
        EIC->BindAction(Inputs.InputActionJump, ETriggerEvent::Started, this, &UCustomLocomotionComponent::Jump);
    }


    if (Inputs.InputActionSprint)
    {
        EIC->BindAction(Inputs.InputActionSprint, ETriggerEvent::Started, this, &UCustomLocomotionComponent::ModifyMove);
        EIC->BindAction(Inputs.InputActionSprint, ETriggerEvent::Completed, this, &UCustomLocomotionComponent::ModifyMove);
    }

    if (Inputs.InputActionCrouch)
    {
        EIC->BindAction(Inputs.InputActionCrouch, ETriggerEvent::Started, this, &UCustomLocomotionComponent::ModifyMove);
    }
}

void UCustomLocomotionComponent::UpdateFallDuration()
{
    FallDuration += GetWorld()->GetDeltaSeconds();
}

void UCustomLocomotionComponent::UnCrouch()
{
    Owner->UnCrouch();
    CharacterMovementStruct.MovementStance = E_CharacterMovementStance::Stand;
    CharacterMovementStruct.MovementGait = E_CharacterMovementGait::Run;
    return;
}


void UCustomLocomotionComponent::StartMove(const FInputActionValue& Value)
{
    const FVector2D MoveInput = Value.Get<FVector2D>();

    if (!IsVectorInputValid(Owner, MoveInput))  
    {
        UE_LOGFMT(LogTemp, Warning, "{0} IsVectorInputValid(Owner, MoveInput) = false", FString(__FUNCTION__));
        return;
    } 
        
    if (MoveInput.Y < 0)
    {
        CharacterMovementComponent->MaxWalkSpeed = BackwordSpeed;
    }
    else if (CharacterMovementStruct.MovementGait == E_CharacterMovementGait::Sprint)
    {
        CharacterMovementComponent->MaxWalkSpeed = SprintSpeed;
    }
    else
    {
        CharacterMovementComponent->MaxWalkSpeed = RunSpeed;
    }

    Owner->AddMovementInput(Owner->GetActorRightVector(), MoveInput.X * MoveInputScale);
    Owner->AddMovementInput(Owner->GetActorForwardVector(), MoveInput.Y * MoveInputScale);  
}

void UCustomLocomotionComponent::HandleLook(const FInputActionValue& Value)
{ 
    const FVector2D LookInput = Value.Get<FVector2D>();

    if (!IsVectorInputValid(Owner, LookInput)
        || !Owner->GetController())
    {
        return;
    }

    Owner->AddControllerYawInput(LookInput.X);
    Owner->AddControllerPitchInput(LookInput.Y);
}

void UCustomLocomotionComponent::ModifyMove(const FInputActionInstance& Instance)
{

    if (Instance.GetSourceAction() == Inputs.InputActionSprint)
    {
        if (Owner->IsCrouched())
        {
            UnCrouch();
            return;
        }

        switch (Instance.GetTriggerEvent())
        { 
        case ETriggerEvent::Started:
            CharacterMovementStruct.MovementGait = E_CharacterMovementGait::Sprint;
            CharacterMovementComponent->MaxWalkSpeed = SprintSpeed;
            break;
        case ETriggerEvent::Completed:
            CharacterMovementStruct.MovementGait = E_CharacterMovementGait::Run;
            CharacterMovementComponent->MaxWalkSpeed = RunSpeed;
            break;
        default:
            break;
        }

        return;
    }


    if (Instance.GetSourceAction() == Inputs.InputActionCrouch)
    {       

        if (Owner->IsCrouched())
        {
            UnCrouch();
            return;
        }
       
        Owner->Crouch();        
        CharacterMovementStruct.MovementStance = E_CharacterMovementStance::Crouch;
        CharacterMovementStruct.MovementGait = E_CharacterMovementGait::Walk;
        return;
    }
}

void UCustomLocomotionComponent::Jump(const FInputActionValue& Value)
{
    if (!Owner->CanJump()) { return; }

    Owner->Jump();
}

void UCustomLocomotionComponent::OnMovementModeChanged(ACharacter* Character, EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
    
    if (CharacterMovementComponent->IsFalling())
    {
        CharacterMovementStruct.MovementMode = E_CharacterMovementMode::InAir;
        FallDuration = 0.0f;     
        GetWorld()->GetTimerManager().SetTimer(FallTimerHandle, this, &UCustomLocomotionComponent::UpdateFallDuration, InFallRate, true);
    }
    else if (PrevMovementMode == MOVE_Falling && CharacterMovementComponent->IsMovingOnGround())
    {
        CharacterMovementStruct.MovementMode = E_CharacterMovementMode::OnGround;
        FallDuration = 0.0f;
        GetWorld()->GetTimerManager().ClearTimer(FallTimerHandle);      
    }
}




///////////////////////////////////////////////////////////////////////////////
//AutoTests

#if WITH_DEV_AUTOMATION_TESTS

const TArray<const UInputAction*> UCustomLocomotionComponent::AutoTestGetInputActions() const
{
    TArray<const UInputAction*> Actions{};
    Actions.Add(Inputs.InputActionMove);
    Actions.Add(Inputs.InputActionLook);

    return Actions;
}

const TArray<FInputMappingContextWithPriority>& UCustomLocomotionComponent::AutoTestGetInputMappingContext() const
{    
    return Inputs.InputMappingContext;
}

#endif //WITH_DEV_AUTOMATION_TESTS
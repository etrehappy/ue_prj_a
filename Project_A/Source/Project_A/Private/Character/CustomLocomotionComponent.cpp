#include "Character/CustomLocomotionComponent.h"
#include "Character/NetPlayerCharacter.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h" 
#include "Net/UnrealNetwork.h"
#include "ProjectALog.h"


////////////////////////////////////////////////////////////////////////////////

UCustomLocomotionComponent::UCustomLocomotionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
    SetIsReplicatedByDefault(true);	
}

void UCustomLocomotionComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UCustomLocomotionComponent, CharacterMovementStruct);
}

void UCustomLocomotionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    CurrentSpeed2D = Owner->GetVelocity().Length();
}

void UCustomLocomotionComponent::BindActions(UEnhancedInputComponent* EIC)
{
    if (IsRunningDedicatedServer())
    {
        UE_LOGFMT(LogProjectA, Warning, "{0} — Running Dedicated Server, skipping this", FString(__FUNCTION__));
        return;
    }

    if (Inputs.InputActionMove)
    {
        EIC->BindAction(Inputs.InputActionMove, ETriggerEvent::Triggered, this, &UCustomLocomotionComponent::StartMoveLocal);
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
        EIC->BindAction(Inputs.InputActionSprint, ETriggerEvent::Started, this, &UCustomLocomotionComponent::ModifyMoveLocal);
        EIC->BindAction(Inputs.InputActionSprint, ETriggerEvent::Completed, this, &UCustomLocomotionComponent::ModifyMoveLocal);
    }

    if (Inputs.InputActionCrouch)
    {
        EIC->BindAction(Inputs.InputActionCrouch, ETriggerEvent::Started, this, &UCustomLocomotionComponent::ModifyMoveLocal);
    }
}

void UCustomLocomotionComponent::BeginPlay()
{
	Super::BeginPlay();

    Initialize();	
}

void UCustomLocomotionComponent::Initialize()
{       
    Owner = Cast<ANetPlayerCharacter>(GetOwner() );
    CharacterMovementComponent = Owner->GetCharacterMovement();

    if (!Owner || !CharacterMovementComponent)
    {
        UE_LOGFMT(LogProjectA, Warning, "{0} — Owner or CharacterMovementComponent = nullptr", FString(__FUNCTION__));
        return;
    }

    Owner->MovementModeChangedDelegate.AddDynamic(this, &UCustomLocomotionComponent::OnMovementModeChanged);   
}

bool UCustomLocomotionComponent::IsVectorInputValid(ACharacter* const OwnerCharacter, const FVector2D& Input) const
{
    if (!OwnerCharacter)
    {
        UE_LOGFMT(LogProjectA, Warning, "{0} — OwnerCharacter = nullptr", FString(__FUNCTION__));
        return false;
    }

    if (Input.IsNearlyZero()) { return false; }

    return true;
}

void UCustomLocomotionComponent::UpdateFallDuration()
{
    FallDuration += GetWorld()->GetDeltaSeconds();
}

void UCustomLocomotionComponent::UpdateMove(const FVector2D& MoveInput)
{
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

void UCustomLocomotionComponent::StartMoveLocal(const FInputActionValue& Value)
{
    const FVector2D MoveInput = Value.Get<FVector2D>();

    if (!IsVectorInputValid(Owner, MoveInput))
    {
        UE_LOGFMT(LogProjectA, Warning, "{0} — IsVectorInputValid(Owner, MoveInput) = false", FString(__FUNCTION__));
        return;
    }

    // client-side movement prediction
    UpdateMove(MoveInput);

    if (!Owner->HasAuthority())
    {
        Server_StartMove(MoveInput);
    }
}

void UCustomLocomotionComponent::ModifyMoveLocal(const FInputActionInstance& Instance)
{
    if (!Owner) return;

    if (Instance.GetSourceAction() == Inputs.InputActionCrouch)
    {
        Crouch();
        return;
    }

    uint8 ActionId = static_cast<uint8>(EInputActionId::None);
    if (Instance.GetSourceAction() == Inputs.InputActionSprint)
    {
        ActionId = static_cast<uint8>(EInputActionId::Sprint);
    }
    Server_ModifyMove(Instance.GetTriggerEvent(), ActionId);
}

void UCustomLocomotionComponent::Crouch()
{
    if (Owner->IsCrouched())
    {
        UnCrouch();
        return;
    }

    Owner->Crouch(); // Owner has movement component, so this will autoreplicate to server
    Server_CrouchState();
    return;
}

void UCustomLocomotionComponent::UnCrouch()
{
    Owner->UnCrouch();
    Server_UnCrouchState();
}

void UCustomLocomotionComponent::HandleLook(const FInputActionValue& Value)
{
    const FVector2D LookInput = Value.Get<FVector2D>();

    if (!IsVectorInputValid(Owner, LookInput)
        || !Owner->GetController())
    {
        UE_LOGFMT(LogProjectA, Warning, "{0} — IsVectorInputValid(Owner, LookInput) = false || !Owner->GetController()", FString(__FUNCTION__));
        return;
    }

    Owner->AddControllerYawInput(LookInput.X);
    Owner->AddControllerPitchInput(LookInput.Y);
}

void UCustomLocomotionComponent::Jump(const FInputActionValue& Value)
{
    if (!Owner->CanJump())
    {
        UE_LOGFMT(LogProjectA, Warning, "{0} — !Owner->CanJump()", FString(__FUNCTION__));
        return;
    }

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

void UCustomLocomotionComponent::Server_StartMove_Implementation(FVector2D MoveInput)
{
    if (!IsVectorInputValid(Owner, MoveInput)) { return; }

    UpdateMove(MoveInput);
}

void UCustomLocomotionComponent::Server_ModifyMove_Implementation(ETriggerEvent Trigger, uint8 ActionId)
{
    if (!Owner) return;

    const EInputActionId Id = static_cast<EInputActionId>(ActionId);

    if (Id == EInputActionId::Sprint)
    {
        if (Owner->IsCrouched())
        {
            UnCrouch();
            return;
        }

        switch (Trigger)
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
}

void UCustomLocomotionComponent::Server_CrouchState_Implementation()
{    
    CharacterMovementStruct.MovementStance = E_CharacterMovementStance::Crouch;
    CharacterMovementStruct.MovementGait = E_CharacterMovementGait::Walk;
}

void UCustomLocomotionComponent::Server_UnCrouchState_Implementation()
{
    CharacterMovementStruct.MovementStance = E_CharacterMovementStance::Stand;
    CharacterMovementStruct.MovementGait = E_CharacterMovementGait::Run;    
}

///////////////////////////////////////////////////////////////////////////////
//AutoTests

#if WITH_DEV_AUTOMATION_TESTS

    const TArray<const UInputAction*> UCustomLocomotionComponent::AutoTestGetInputActions() const
{
    TArray<const UInputAction*> Actions{};
    Actions.Add(Inputs.InputActionMove);
    Actions.Add(Inputs.InputActionLook);
    Actions.Add(Inputs.InputActionSprint);
    Actions.Add(Inputs.InputActionJump);
    Actions.Add(Inputs.InputActionCrouch);    

    return Actions;
}

#endif //WITH_DEV_AUTOMATION_TESTS
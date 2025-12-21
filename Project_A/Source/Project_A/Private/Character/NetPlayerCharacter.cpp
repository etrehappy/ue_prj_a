#include "Character/NetPlayerCharacter.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "ProjectALog.h"

ANetPlayerCharacter::ANetPlayerCharacter()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
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
}

void ANetPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ANetPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {

        LocomotionComponent->BindActions(EnhancedInputComponent);
	}
	else
	{
        UE_LOGFMT(LogProjectA, Error, "{0} —  Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file.", *GetNameSafe(this));		
	}
    
}

void ANetPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
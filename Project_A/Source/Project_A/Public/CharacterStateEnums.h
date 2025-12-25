#pragma once

#include "CoreMinimal.h"
#include "CharacterStateEnums.generated.h"


UENUM(BlueprintType)
enum class E_CharacterMovementState : uint8
{
    Idle            UMETA(DisplayName = "Idle")
    , Moving        UMETA(DisplayName = "Moving")
};

UENUM(BlueprintType)
enum class E_CharacterMovementMode : uint8
{
    OnGround        UMETA(DisplayName = "OnGround")
    , InAir         UMETA(DisplayName = "InAir")
};

UENUM(BlueprintType)
enum class E_CharacterMovementStance : uint8
{
    Stand           UMETA(DisplayName = "Stand")
    , Crouch        UMETA(DisplayName = "Crouch")
};

UENUM(BlueprintType)
enum class E_CharacterMovementGait : uint8
{

    Run             UMETA(DisplayName = "Run")
    , Walk          UMETA(DisplayName = "Walk")
    , Sprint        UMETA(DisplayName = "Sprint")
};

UENUM(BlueprintType)
enum class E_CharacterMovementDirection : uint8
{
    None            UMETA(DisplayName = "None")
    , Forward       UMETA(DisplayName = "Forward")
    , Backward      UMETA(DisplayName = "Backward")
    //, Sideways      UMETA(DisplayName = "Sideways")
    , Right      UMETA(DisplayName = "Right")
    , Left      UMETA(DisplayName = "Left")
};

USTRUCT(BlueprintType)
struct FCharacterMovementStruct
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement")
    E_CharacterMovementState State{E_CharacterMovementState::Idle};

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement")
    E_CharacterMovementMode Mode{E_CharacterMovementMode::OnGround};

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement")
    E_CharacterMovementStance Stance{E_CharacterMovementStance::Stand};

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement")
    E_CharacterMovementGait Gait{E_CharacterMovementGait::Run};

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement")
    E_CharacterMovementDirection Direction{E_CharacterMovementDirection::None};

};
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

USTRUCT(BlueprintType)
struct FCharacterMovementStruct
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement")
    E_CharacterMovementMode MovementMode{E_CharacterMovementMode::OnGround};

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement")
    E_CharacterMovementStance MovementStance{E_CharacterMovementStance::Stand};

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement")
    E_CharacterMovementGait MovementGait{E_CharacterMovementGait::Run};

};
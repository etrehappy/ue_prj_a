#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Character/BaseCharacter.h"
#include "Character/CustomLocomotionComponent.h"
#include "Character/CustomPlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "PlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UArrowComponent;

UCLASS()
class PROJECT_A_API APlayerCharacter : public ABaseCharacter
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APlayerCharacter();

	// Called every frame
	virtual void Tick(float DeltaTime) override;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom{};

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera{};

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UArrowComponent* Arrow{};

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Components")
	UCustomLocomotionComponent* LocomotionComponent{};

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Components")
	ACustomPlayerController* PlayerController{};

};

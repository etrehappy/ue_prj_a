#pragma once
#include "CoreMinimal.h"

#include "Character/BaseCharacter.h"
#include "Character/CustomLocomotionComponent.h"
#include "Character/CustomPlayerController.h"
#include "WeaponComponent.h"

#include "NetPlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UArrowComponent;
class UBattleComponent;

UCLASS()
class PROJECT_A_API ANetPlayerCharacter : public ABaseCharacter
{
	GENERATED_BODY()
	
public:	
	ANetPlayerCharacter();
	virtual ~ANetPlayerCharacter() override = default;
	virtual void Destroyed() override;
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	/** Initialize input action bindings */
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	UFUNCTION(BlueprintImplementableEvent, Category = "Events")
	void OnEquipWeapon();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom{};

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera{};

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UArrowComponent* Arrow{};

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Components")
	UCustomLocomotionComponent* LocomotionComponent{};

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Components")
	UBattleComponent* BattleComponent{};

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Components")
	UWeaponComponent* WeaponComponent{};

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Components")
	ACustomPlayerController* PlayerController{};	

};

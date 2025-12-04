#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Character/BaseCharacter.h"
#include "Character/CustomPlayerController.h"
#include "CharacterStateEnums.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "Logging/StructuredLog.h"

#include "CustomLocomotionComponent.generated.h"


DECLARE_LOG_CATEGORY_EXTERN(LogMyGame, Log, All);

////////////////////////////////////////////////////////////////////////////////

class UInputAction;

USTRUCT(BlueprintType)
struct FInputMappingContextWithPriority
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UInputMappingContext* MappingContext = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Priority = 0;
};

USTRUCT(BlueprintType)
struct FInputStruct
{
	GENERATED_BODY()

	/**
	 * @brief Which keys or axes trigger which actions.
	 * @note This field must be set before running the game.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite/*, Category = "Input", meta = (AllowPrivateAccess = "true")*/)
	TArray<FInputMappingContextWithPriority> InputMappingContext{};

	/**
	 * @brief Which action will be processed by this component (and which function).
	 * @note This field must be set before running the game.
	 */
	UPROPERTY(EditDefaultsOnly/*, Category = "Input", meta = (AllowPrivateAccess = "true")*/)
	UInputAction* InputActionMove{};

	UPROPERTY(EditDefaultsOnly/*, Category = "Input", meta = (AllowPrivateAccess = "true")*/)
	UInputAction* InputActionLook{};

	UPROPERTY(EditDefaultsOnly/*, Category = "Input", meta = (AllowPrivateAccess = "true")*/)
	UInputAction* InputActionSprint{};

	UPROPERTY(EditDefaultsOnly/*, Category = "Input", meta = (AllowPrivateAccess = "true")*/)
	UInputAction* InputActionJump{};

	UPROPERTY(EditDefaultsOnly/*, Category = "Input", meta = (AllowPrivateAccess = "true")*/)
	UInputAction* InputActionCrouch{};

};



////////////////////////////////////////////////////////////////////////////////

/**
* @brief Collection of Input Mapping Contexts required for character movement.
*/
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECT_A_API UCustomLocomotionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCustomLocomotionComponent();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, 
		FActorComponentTickFunction* ThisTickFunction) override;	


#if WITH_DEV_AUTOMATION_TESTS
public:
	const TArray<const UInputAction*> AutoTestGetInputActions() const;
	const TArray<FInputMappingContextWithPriority>& AutoTestGetInputMappingContext() const;
#endif // WITH_DEV_AUTOMATION_TESTS

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
	UPROPERTY(EditDefaultsOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	FInputStruct Inputs{};	

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input", meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float MoveInputScale;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement")
	FCharacterMovementStruct CharacterMovementStruct{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement")
	float BackwordSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement")
	float RunSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement")
	float SprintSpeed;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	float FallDuration{};

	UPROPERTY(BlueprintReadOnly)
	double CurrentSpeed2D{};

private:
	/**
	* @brief Iterates InputMappingContexs and calls CustomPC->AddInputMappingContext(...) 
	* to register each mapping context with the player’s Enhanced Input subsystem. 
	* Those contexts are what actually translate key presses to InputActions.
	*/
	void Initialize();
	ACustomPlayerController* GetCustomPlayerController() const;
	bool IsVectorInputValid(ACharacter* const, const FVector2D&) const;
	void BindActions(UEnhancedInputComponent* EIC);
	void UpdateFallDuration();
	void UnCrouch();

	UFUNCTION()
	void StartMove(const FInputActionValue& Value);

	UFUNCTION()
	void HandleLook(const FInputActionValue& Value);

	UFUNCTION()
	void ModifyMove(const FInputActionInstance& Instance);

	UFUNCTION()
	void Jump(const FInputActionValue& Value);

	UFUNCTION()
	void OnMovementModeChanged(ACharacter* Character, EMovementMode PrevMovementMode,
		uint8 PreviousCustomMode);

	ABaseCharacter* Owner{};	
	ACustomPlayerController* CustomPC{};
	UCharacterMovementComponent* CharacterMovementComponent{};

	FTimerHandle FallTimerHandle{};
	const float InFallRate{0.01f};

	
};
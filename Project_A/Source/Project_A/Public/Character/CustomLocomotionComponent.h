#pragma once


#include "Components/ActorComponent.h"
#include "CoreMinimal.h"
#include "Character/BaseCharacter.h"
#include "CharacterStateEnums.h"
#include "EnhancedInputComponent.h"
#include "InputAction.h"
#include "InputActionValue.h"

#include "CustomLocomotionComponent.generated.h"




////////////////////////////////////////////////////////////////////////////////

class UInputAction;
class ANetPlayerCharacter;


USTRUCT(BlueprintType)
struct FInputStruct
{
	GENERATED_BODY()

	/**
	 * @brief Which action will be processed by this component (and which function).
	 * @note This field must be set before running the game.
	 */
	UPROPERTY(EditDefaultsOnly)
	UInputAction* InputActionMove{};

	UPROPERTY(EditDefaultsOnly)
	UInputAction* InputActionLook{};

	UPROPERTY(EditDefaultsOnly)
	UInputAction* InputActionSprint{};

	UPROPERTY(EditDefaultsOnly)
	UInputAction* InputActionJump{};

	UPROPERTY(EditDefaultsOnly)
	UInputAction* InputActionCrouch{};

};

UENUM()
enum class EInputActionId : uint8
{
	None = 0
	, Sprint
	, Crouch
};

////////////////////////////////////////////////////////////////////////////////

/**
* @brief Collection of Input Mapping Contexts required for character movement.
*/
UCLASS( ClassGroup=(Moving), meta=(BlueprintSpawnableComponent) )
class PROJECT_A_API UCustomLocomotionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UCustomLocomotionComponent();
	virtual ~UCustomLocomotionComponent() = default;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	void BindActions(UEnhancedInputComponent* EIC);

protected:
	virtual void BeginPlay() override;

private:
	void Initialize();
	bool IsVectorInputValid(ACharacter* const, const FVector2D&) const;
	void UpdateFallDuration();	
	void UpdateMove(const FVector2D& MoveInput);
	void StartMoveLocal(const FInputActionValue& Value);
	void ModifyMoveLocal(const FInputActionInstance& Instance);

	/*!
	* @brief Owner has character movement component, so this will autoreplicate to server
	*/
	void Crouch();

	/*!
	* @brief Owner has character movement component, so this will autoreplicate to server
	*/
	void UnCrouch();

	ANetPlayerCharacter* Owner{};
	UCharacterMovementComponent* CharacterMovementComponent{};
	FTimerHandle FallTimerHandle{};
	const float InFallRate{0.01f};

public:
	//

protected:	
	UPROPERTY(EditDefaultsOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	FInputStruct Inputs{};

	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadWrite, Category = "Movement")
	FCharacterMovementStruct CharacterMovementStruct{};

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	float FallDuration{};

	UPROPERTY(BlueprintReadOnly)
	double CurrentSpeed2D{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input", 
		meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float MoveInputScale{1.f};

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement")
	float BackwordSpeed{200.f};

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement")
	float RunSpeed{350.f};

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement")
	float SprintSpeed{500.f};

private:
	UFUNCTION()
	void HandleLook(const FInputActionValue& Value);

	UFUNCTION()
	void Jump(const FInputActionValue& Value);

	UFUNCTION()
	void OnMovementModeChanged(ACharacter* Character, EMovementMode PrevMovementMode, uint8 PreviousCustomMode);
	
	UFUNCTION(Server, Reliable)
	void Server_StartMove(FVector2D MoveInput);	

	UFUNCTION(Server, Reliable)
	void Server_ModifyMove(ETriggerEvent Trigger, uint8 ActionId);

	/*!
    * @brief Server function just update Enums. 
	*/
	UFUNCTION(Server, Reliable)
	void Server_CrouchState();

	/*!
	* @brief Server function just update Enums.
	*/
	UFUNCTION(Server, Reliable)
	void Server_UnCrouchState();



#if WITH_DEV_AUTOMATION_TESTS
public:
	const TArray<const UInputAction*> AutoTestGetInputActions() const;

#endif // WITH_DEV_AUTOMATION_TESTS

};
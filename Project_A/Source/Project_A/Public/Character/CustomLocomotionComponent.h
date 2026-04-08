#pragma once

#include "CoreMinimal.h"

#include "Character/BaseCharacter.h"
#include "CharacterStateEnums.h"
#include "Components/ActorComponent.h"
#include "EnhancedInputComponent.h"
#include "InputAction.h"
#include "InputActionValue.h"
#include "StatusEffect/StatusEffectStatHandler.h"

#include "CustomLocomotionComponent.generated.h"

class UInputAction;
class ANetPlayerCharacter;


/**
 * @struct FInputStruct
 * @brief Which action will be processed by this component (and which function).
 * @note This field must be set before running the game.
 */
USTRUCT(BlueprintType)
struct FInputStruct
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputAction> InputActionMove{};

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputAction> InputActionLook{};

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputAction> InputActionSprint{};

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputAction> InputActionJump{};

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputAction> InputActionCrouch{};

};


////////////////////////////////////////////////////////////////////////////////

/**
 * @class UCustomLocomotionComponent
 * @brief This class does not replace ACharacterMovementComponent, but works with it.
 * 
 * @details Responsibilities:
 *	- Process player input for movement and look direction.
 *	- Update character movement states (e.g., walking, running, sprinting, crouching, etc.).
 *	- Validate canMove state before applying movement.
 * 
 * Network: available
 * 
 * @see UCustomInputComponent
 * @todo Transfer input logic and Input Mapping Contexts to 'UCustomInputComponent'.
 * @todo Implement logic for CanMove
 * @todo Restrict camera's rotation along the Z axis.
 */
UCLASS( ClassGroup=(Moving), meta=(BlueprintSpawnableComponent) )
class PROJECT_A_API UCustomLocomotionComponent : public UActorComponent, public IStatusEffectStatHandler
{
	GENERATED_BODY()

						/**  === C++ member functions === */
public:

	UCustomLocomotionComponent();
	virtual ~UCustomLocomotionComponent() = default;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;	

	/**
	 * @brief Binds input actions to the enhanced input component.
	 * 
	 * @param[out] EIC The enhanced input component to bind actions to.
	 * @see ACustomPlayerController::SetupInputComponent
	 * @todo Replace to UCustomInputComponent
	 */
	void BindActions(UEnhancedInputComponent* EIC);
	
	/**
	 * @see IStatusEffectStatHandler
	 */
	virtual bool CanHandleStatTag(const FGameplayTag& StatTag) const override;

	/**
	 * @see IStatusEffectStatHandler
	 */
	virtual void ApplyStatusEffectAction(const FEffectAction& Action) override;

	/**
	 * @see IStatusEffectStatHandler
	 */
	virtual void RemoveStatusEffectAction(const FEffectAction& Action) override;


protected:
	virtual void BeginPlay() override;

private:
	/**
	 * @brief Sets up initial references. Subscribes UCustomLocomotionComponent::OnMovementModeChanged to Character's MovementModeChangedDelegate 
	 */
	void Initialise();

	/**
	 * @brief Validates the input vector for movement or look direction.
	 * 
	 * @param[in] Owner The character owning this component.
	 * @param[in] Input The input (wasd, camera) vector to validate.
	 * @return False if the input vector is nearly zero.
	 */
	bool IsVectorInputValid(ACharacter* const Owner, const FVector2D& Input) const;

	/**
	 * @brief Incrementally updates the fall duration timer each frame while the character is in the air.
	 *  Timer started on movement-mode change to falling.
	 * @see FallDuration
	 */
	void UpdateFallDuration();	

	/**
	 * @brief Used by Server and Client. The client predicts movement, the server validates it.
	 * 
	 * @param[in] MoveInput The movement input vector.
	 */
	void UpdateMove(const FVector2D& MoveInput);

	/**
	 * @brief Change E_CharacterMovementState that used in a ChooserTable
	 * @see CharacterMovementStruct
	 */
	void UpdateDirection();

	/**
	 * @brief Predicts movement and calls RPC to server to validate movement.
	 */
	void StartMoveLocal(const FInputActionValue& Value);

	/**
	 * @brief Predicts MovementState and calls RPC to server to validate movement.
	 */
	void ModifyMoveLocal(const FInputActionInstance& Instance);

	/**
	 * @brief Client only.
	 *
	 * @param[in] Value The look input vector.
	 */
	void HandleLook(const FInputActionValue& Value);

	/**
	 * @brief Used by Server and Client. The client predicts movement state, the server validates it.
	 */
	bool IsSprintAvailable() const;

	/**
	 * @brief Owner has character movement component, so this will autoreplicate to server
	 */
	void Crouch();

	/**
	 * @brief Owner has character movement component, so this will autoreplicate to server
	 */
	void UnCrouch();

	/**
	 * @brief Owner has character movement component, so this will autoreplicate to server
	 */
	void Jump(const FInputActionValue& Value);



						/* === C++ member variables === */
private:
	ANetPlayerCharacter* Owner{};
	UCharacterMovementComponent* CharacterMovementComponent{};

	/**
	 * @see OnMovementModeChanged
	 */
	FTimerHandle FallTimerHandle{};

	/**
	 * @see OnMovementModeChanged
	 */
	const float InFallRate{0.01f};

	/**
	 * @see UpdateDirection
	 */	
	const float Tolerance{0.0001f};

	/**
	 * @see UpdateDirection
	 */
	const float MinDotProductRange{-0.1f};

	/**
	 * @see UpdateDirection
	 */	
	const float MaxDotProductRange{0.1f};


					/* === Unreal Engine UFUNCTION === */
private:
	/**
	 * @brief Client only.
	 * Handles changes in the character's movement mode (e.g., walking, falling). 
	 * Responds to MovementModeChangedDelegate. Should be UFUNCTION to be bound to delegate.
	 *
	 * @param[in] PrevMovementMode The previous movement mode.
	 */
	UFUNCTION()
	void OnMovementModeChanged(ACharacter* Character, EMovementMode PrevMovementMode, uint8 PreviousCustomMode);

	UFUNCTION(Server, Reliable)
	void Server_StartMove(FVector2D MoveInput);

	UFUNCTION(Server, Reliable)
	void Server_ModifyMove(ETriggerEvent Trigger, uint8 ActionId);

	/**
	 * @brief Server function just update Enums.
	 */
	UFUNCTION(Server, Reliable)
	void Server_CrouchState();

	/**
	 * @brief Server function just update Enums.
	 */
	UFUNCTION(Server, Reliable)
	void Server_UnCrouchState();


					/* === Unreal Engine UPROPERTY === */
private:
	/**
	 * @brief Needs to be set up in the Blueprint before the game is run.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	FInputStruct Inputs{};

	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	FCharacterMovementStruct CharacterMovementStruct{};

	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float FallDuration{};

	/**
	 * @brief Used by UpdateDirection() and by AnimBlueprints.
	 */
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	double CurrentSpeed2D{};

	/**
	 * @brief This is not relevant for the current implementation, but it may be used in the future to predict movement.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input", 
		meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0", AllowPrivateAccess = "true"))
	float MoveInputScale{1.f};

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Replicated, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float BackwordSpeed{200.f};

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Replicated,	Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float RunSpeed{350.f};

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Replicated, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float SprintSpeed{500.f};

	/**
	 * @brief Tags of stats that this component can handle. Should be set up in the Blueprint before the game is run.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "StatusEffect")
	FGameplayTagContainer SupportedStatTags{};


#if WITH_DEV_AUTOMATION_TESTS
public:
	const TArray<const UInputAction*> AutoTestGetInputActions() const;

#endif // WITH_DEV_AUTOMATION_TESTS

};

//void SetCanMove(const bool bNewCanMove = true);
//bool GetCanMove() const;
//UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Replicated, Category = "Movement", meta = (AllowPrivateAccess = "true"))
//bool bCanMove{true};
//
//public:
//	/**
//	 * @brief Used to get current character movement struct in Blueprint.
//	 *
//	 * @see FCharacterMovementStruct
//	 */
//	UFUNCTION()
//	FCharacterMovementStruct GetCharacterMovementStruct() const;

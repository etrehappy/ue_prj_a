/*****************************************************************//**
 * \file   CustomInputComponent.h
 * \brief  The general input-manager for a player character.
 * 
 * \date   January 2026
 *********************************************************************/
#pragma once

#include "CoreMinimal.h"

#include "Components/ActorComponent.h"
#include "EnhancedInputComponent.h"
#include "InputAction.h"
#include "InputActionValue.h"

#include "CustomInputComponent.generated.h"


class ANetPlayerCharacter;

/**
 * @enum EInputActionId
 * @brief It was created because of throwable items only. It may be used in future for other input actions.
 * 
 * @date January 2026
 * @todo Check whether it used somewhere else and remove it if not.
 */
UENUM(BlueprintType)
enum class EInputActionId : uint8
{
	None = 0
	, Sprint
	, Crouch
	, EquipWeapon
	, SimpleAttack
	, ThrowThrowableItem
	, EquipThrowableItem

	, End
};

/**
 * @struct FInputActionStruct
 * @brief The inputs that can be used by the character. 
 */
USTRUCT()
struct FInputActionStruct
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, Category = "Action")
	TObjectPtr<UInputAction> EquipWeapon{};

	UPROPERTY(EditDefaultsOnly, Category = "Action")
	TObjectPtr<UInputAction> SimpleAttack{};

	UPROPERTY(EditDefaultsOnly, Category = "Action")
	TObjectPtr<UInputAction> ThrowThrowableItem{};

	UPROPERTY(EditDefaultsOnly, Category = "Action")
	TObjectPtr<UInputAction> EquipThrowableItem{};

	UPROPERTY(EditDefaultsOnly, Category = "Action")
	TObjectPtr<UInputAction> Interaction{};

	UPROPERTY(EditDefaultsOnly, Category = "Menu")
	TObjectPtr<UInputAction> ToggleInventory{};
};


/**
 * @brief The general input-manager for a player character. 
 * Current implementation is hard-linked to the owner.
 * 
 * @details Responsibilities:
 * - Bind input actions to functions in code.
 * - Handle input actions and call appropriate functions in the player character.
 * 
 * @todo Check is it necessary to broadcast events to other components.
 */
UCLASS(Blueprintable, ClassGroup=(CustomInput), meta=(BlueprintSpawnableComponent) )
class PROJECT_A_API UCustomInputComponent : public UActorComponent
{
	GENERATED_BODY()

						/* === C++ member functions === */
public:	
	UCustomInputComponent();
	virtual ~UCustomInputComponent() = default;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/**
	 * @brief Binds input actions to the enhanced input component.
	 *
	 * @param[out] EIC The enhanced input component to bind actions to.
	 * @see ACustomPlayerController::SetupInputComponent
	 */
	void BindActions(UEnhancedInputComponent* EIC);

protected:	
	virtual void BeginPlay() override;

private:
	/**
	 * @brief Sets the Owner
	 */
	void Initialise();

	/**
	 * @brief Client only. 
	 * It simply informs the character about the call.
	 */
	void EquipWeapon(const FInputActionValue& Value);

	/**
	 * @brief Client only. 
	 * The character begins to aim when a throwable item is equipped; therefore, the current implementation also calls 'HandleAbility'. 
	 * 
	 * @see HandleAbility
	 * @see WeaponThrowable.h
	 */
	void EquipThrowableItem(const FInputActionInstance& Instance);


	/**
	 * @brief Client only.
	 * It triggers a chain of checks for create an action like "simple attack", "throw item", "aim" and so on.
	 * 
	 * @param[in] Instance lets get more information about the input action.
	 * @see AbilityComponent.h
	 * @see BaseAbility.h
	 * 
	 */
	void HandleAbility(const FInputActionInstance& Instance);

	/**
	 * @brief Client only.
	 * 
	 */
	void Interaction(const FInputActionValue& Value);

	/**
	 * @brief 
	 * 	
	 */
	void ToggleInventory(const FInputActionValue& Value);

						/* === C++ member === */

	ANetPlayerCharacter* Owner{};


						/* === Unreal Engine UPROPERTY === */
private:
	/**
	 * @brief Needs to be set up in the Blueprint with input actions to bind them in code.
	 * @see FInputActionStruct
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	FInputActionStruct InputActions{};
};

/*****************************************************************//**
 * \file   NetPlayerCharacter.h
 * \brief  This is the networked player character class that extends the base character functionality.
 *
 * \date   January 2026
 *********************************************************************/

#pragma once
#include "CoreMinimal.h"

#include "AbilityComponent.h"
#include "Character/BaseCharacter.h"
#include "Character/CombatComponent.h"
#include "Character/CustomInputComponent.h"
#include "Character/CustomLocomotionComponent.h"
#include "Character/CustomPlayerController.h"
#include "WeaponComponent.h"
#include "PickUpInterface.h"

#include "NetPlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UArrowComponent;
class UHealthComponent;
class UInventoryComponent;
class UInteractionComponent;
class UStatusEffectComponent;

class UInventory;

/**
 * @class ANetPlayerCharacter
 * @brief It is a main class for player-controlled characters in this project.
 * 
 * @details Responsibilities:
 *  - Manage player components: `UAbilityComponent`, `UWeaponComponent`, `UCustomInputComponent`,
 *    `UCustomLocomotionComponent`, and `UCombatComponent`.
 * 
 */
UCLASS()
class PROJECT_A_API ANetPlayerCharacter : public ABaseCharacter, public  IPickUpInterface
{
    GENERATED_BODY()

                        /* === C++ member functions === */
public:
    ANetPlayerCharacter();


    virtual ~ANetPlayerCharacter() override = default;
    /**
     * @brief Deletes a weapon on the server if it was equipped when the character was destroyed.
     */
    virtual void Destroyed() override;
    virtual void Tick(float DeltaTime) override;
    

    /**
     * @brief Client function.
     *	Handles equipping and unequipping of weapons and throwable items based on the input action type.
     */
    void EquipWeapon(EInputActionId InputActionType);	

    /**
     * @brief Client function.
     * 
     * For input action
     */
    void Interact();

    /**
     * @brief Client function.
     * 
     * For input action
     */
    void ToggleInventory();

protected:

    virtual void BeginPlay() override;

    /**
     * @brief Initializes input action bindings for the player character.
     *
     * @param[in] PlayerInputComponent The input component to bind actions to.
     * @see ACustomPlayerController::SetupInputComponent
     */
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    /**
     * @brief 
     * 
     * @param[in] NewFocusedActor
     * @see UInteractionComponent
     */
    virtual void HandleFocusChanged(AActor* NewFocusedActor);

    /**
     * @brief Disables the character's control, preventing any input actions from being processed.
     */
    void DisableCharacterControl();

    /**
     * @brief Restores the character's collision settings to their default state.
     * 
     */
    void RestoreCollision();

                        /* === Unreal Engine UFUNCTION === */
protected:

    UFUNCTION()
    void OtherBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
    void OtherEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

    /**
     * @brief Client function.
     * It should be implemented in Blueprint to handle visual and audio effects when equipping a weapon
     *	or a throwable item. It is called from C++ by EquipWeapon function that manages equipping and unequipping
     *	of weapons and throwable items based on the input action type.
     *
     * @param[in] InputActionType It may be used to equip a base weapon or a throwable item.
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Events")
    void OnEquipWeapon(const EInputActionId InputActionType);/*TODO Check Net*/

    /**
     * @brief Server function.
     * It handles the activation of abilities on the server side. Character is subscribed to the 'OnAbilityActivated' event of the 'AbilityComponent'.
     *
     * @param[in] Ability The gameplay tag representing the activated ability. It may be "aim to throw item", "throw item", "simple attack", etc.
     */
    UFUNCTION()
    void HandleAbilityActivated(FGameplayTag Ability);

    UFUNCTION(NetMulticast, Unreliable)
    void Multicast_PlayHealFX(float HealAmount);

    UFUNCTION(NetMulticast, Unreliable)
    void Multicast_PlayDamageFX(float DamageAmount);

    UFUNCTION(NetMulticast, Reliable)
    void Multicast_PlayDeathFX();
    
    UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, Category = "Events")
    void OnPlayHealFX(float HealAmount);

    UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, Category = "Events")
    void OnPlayDamageFX(float DamageAmount);

    UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, Category = "Events")
    void OnPlayDeathFX();

    /**
     * @brief Client function.
     * 
     * It is called in the character's Blueprint when the death effects have finished playing on the client side.
     */
    UFUNCTION(Server, Reliable, BlueprintCallable)
    void Server_OnDeathFxFinished();

    virtual bool PickUpItem_Implementation(AItemPickup* Item) override;
    
private:
    /**
     * @brief Server function.
     * It handles the character's death on the server side.
     */
    UFUNCTION()
    void OnDead();

    /**
     * @brief Server function.
     */
    UFUNCTION()
    void OnIncreaseHealth(float HealAmount);

    /**
     * @brief Server function.
     */
    UFUNCTION()
    void OnDecreaseHealth(float DamageAmount);

    /**
	 * @brief Client function.
     */
    UFUNCTION()
    void HandleHealthChanged(float CurrentHealthValue, float MaxHealthValue);


                        /* === Unreal Engine UPROPERTY === */
protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Camera", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<USpringArmComponent> CameraBoom{};

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Camera", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UCameraComponent> FollowCamera{};

    UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Camera", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UArrowComponent> Arrow{};

    /**
     * @brief Manages character movement, including walking, running, jumping, and other locomotion-related functionalities.
     */
    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UCustomLocomotionComponent> LocomotionComponent{};

    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UCustomInputComponent> CustomInputComponent{};

    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UWeaponComponent> WeaponComponent{};

    /**
     * @brief Ability is used to manage whether the character can perform certain action. Abilities are managed through the AbilityComponent.
     */
    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UAbilityComponent> AbilityComponent{};

    /**
     * @brief It manages current combat conditions, such as whether the character is equipped with a weapon and whether the character is fighting.
     */
    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UCombatComponent> CombatComponent{};
        
    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UHealthComponent> HealthComponent{};

    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UInventoryComponent> InventoryComponent{};

    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UInteractionComponent> InteractionComponent{};

    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UStatusEffectComponent> StatusEffectComponent{};

    /**
     * @brief The temporary solution. 
     * @todo Update with Interact system
     */
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Interaction")
    TObjectPtr<AActor> CurrentInteractable{};

};


//UFUNCTION(BlueprintCallable)
//void UnequipAnyWeapon(const EInputActionId InputActionType);

//virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
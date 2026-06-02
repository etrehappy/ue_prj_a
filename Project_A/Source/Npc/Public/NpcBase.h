/*****************************************************************//**
 * \file   NpcBase.h
 * \brief  Base class for all NPCs. Provides common functionality such as health, status effects, and combat behavior.
 *
 * \date   April 2026
 *********************************************************************/
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "HealthComponent.h"
#include "NpcTypes.h"
#include "Interactable.h"
#include "InteractionInventoryProvider.h"

#include "NpcBase.generated.h"

class UStatusEffectComponent;
class UNpcBattleComponent;
class UDialogueDefinition;
class UInventoryComponent;

/**
 * @class ANpcBase
 * @brief It is a base class for all NPC
 */
UCLASS(Abstract)
class NPC_API ANpcBase : public ACharacter, public IInteractable, public IInteractionInventoryProvider
{
	GENERATED_BODY()

	/* === C++ member functions === */
public:
	ANpcBase();
	virtual ~ANpcBase() override = default;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void SetIsAttacking(bool bNewIsAttacking);

	// IInteractable
	virtual bool CanInteract(APawn* Interactor) const override;
	virtual void BuildInteractionActions(APawn* Interactor, TArray<FInteractionActionType>& OutActions) const override;
	virtual bool ExecuteInteractionAction(APawn* Interactor, FGameplayTag ActionTag) override;

	// IInteractionInventoryProvider
	virtual UInventoryComponent* GetInventoryComponent() const override;

protected:
	// BP hooks
	UFUNCTION(BlueprintImplementableEvent, Category = "Interaction")
	bool BP_CanInteract(APawn* Interactor) const;

	UFUNCTION(BlueprintImplementableEvent, BlueprintAuthorityOnly, Category = "Interaction")
	bool BP_ExecuteInteractionAction(APawn* Interactor, FGameplayTag ActionTag);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	bool bCanInteractWhenAlive{false};

	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	bool bCanInteractWhenDead{false};

	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	float InteractionDistance{250.f};

	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	float InteractionAngle{70.f};

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Inventory")
	TObjectPtr<UInventoryComponent> InventoryComponent{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory")
	bool bSupportsInventoryInteraction{false};

protected:
	virtual void BeginPlay() override;
	void DestroyNpc();

private:
	bool HasDirectDialogue() const;
	bool TryStartDialogueDirect(APawn* Interactor) const;

	/* === C++ member variables === */
private:
	/**
	 * @brief Manages the NPC's death.
	 */
	FTimerHandle DeathTimerHandle{};

	/* === Unreal Engine UFUNCTION === */
public:
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void SetNpcCoreData(float InMaxHealth, float InDamage, float InMoveSpeed, ENpcFaction InFaction);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void SetCurrentTarget(AActor* NewTarget);

	UFUNCTION(BlueprintPure)
	AActor* GetCurrentTarget() const { return CurrentTarget; }

	UFUNCTION(BlueprintPure)
	ENpcFaction GetNpcFaction() const { return NpcFaction; }

	/**
	 * @see UNpcBattleComponent::Multicast_PlayAttack
	 */
	UFUNCTION(BlueprintImplementableEvent)
	void OnPlayAttack();

	UFUNCTION(BlueprintPure)
	bool IsDead() const { return bIsDead; }

public:
	bool CanAcceptSharedTarget(const AActor* NewTarget) const;
	bool TryApplySharedTarget(AActor* NewTarget, const AActor* SignalSource);

protected:
	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_PlayHealFX(float HealAmount);

	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_PlayDamageFX(float DamageAmount);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayDeathFX();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic)
	void OnPlayHealFX(float HealAmount);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic)
	void OnPlayDamageFX(float DamageAmount);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic)
	void OnPlayDeathFX();

	UFUNCTION(BlueprintImplementableEvent)
	void OnCurrentTargetChanged(AActor* NewTarget);

private:
	UFUNCTION()
	void OnRep_CurrentTarget();

	UFUNCTION()
	void OnIncreaseHealth(float HealAmount);

	UFUNCTION()
	void OnDecreaseHealth(float DamageAmount);

	UFUNCTION()
	void OnDead();

	/* === Unreal Engine UPROPERTY === */
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UHealthComponent> HealthComponent{};

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UStatusEffectComponent> StatusEffectComponent{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated)
	TObjectPtr<UNpcBattleComponent> NpcBattleComponent{};

	/**
	 * @brief Used by StateTree task.
	 * @see UNpcBattleComponent::SetIsAttacking
	 * @see UNpcBattleComponent::bIsAttacking
	 * @todo Check how the project works without this variable.
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Replicated)
	bool bIsAttacking{false};

	UPROPERTY(EditDefaultsOnly, Replicated, BlueprintReadOnly)
	float MoveSpeed{300.f};

	UPROPERTY(EditDefaultsOnly, Replicated, BlueprintReadOnly)
	ENpcFaction NpcFaction{ENpcFaction::Enemy};

	UPROPERTY(EditDefaultsOnly, Replicated, BlueprintReadOnly)
	bool bIsDead{false};

	/**
	 * @brief The time after which the NPC will be destroyed after death.
	 */
	UPROPERTY(EditDefaultsOnly)
	float DeathLifeSpan{120.0f};

	/**
	 * @brief The current target of the NPC.
	 */
	UPROPERTY(VisibleInstanceOnly, ReplicatedUsing = OnRep_CurrentTarget, BlueprintReadOnly)
	TObjectPtr<AActor> CurrentTarget{};

	/**
	 * @brief for NPCs without direct dialogue.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction")
	TArray<TObjectPtr<UInteractionActionDefinition>> InteractionActions{};

	/**
	 * @brief If set, interaction starts dialogue immediately without interaction menu.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dialogue")
	TObjectPtr<UDialogueDefinition> DialogueDefinition{};
};
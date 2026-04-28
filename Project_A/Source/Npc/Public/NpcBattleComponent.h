/*****************************************************************//**
 * \file   NpcBattleComponent.h
 * \brief  Component responsible for handling NPC battle logic, including attacks and damage. 
 * NPCs have different skeletal meshes so WeaponComponent is not suitable for them.
 * 
 * \date   April 2026
 *********************************************************************/
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameFramework/DamageType.h"

#include "NpcBattleComponent.generated.h"

class UNpcAttackLogicBase;
class ANpcBase;

/**
 * @class UNpcBattleComponent
 * @brief Simple solution for NPCs.
 * 
 * @see UNpcAttackLogicBase - defines the logic of how the attack works (e.g., melee, ranged, etc.).
 * @see ANpcBase, AEnemyBase
 */
UCLASS(ClassGroup = (CustomNpcBattle), meta = (BlueprintSpawnableComponent))
class NPC_API UNpcBattleComponent : public UActorComponent
{
	GENERATED_BODY()

						/* === C++ member functions === */
public:
	UNpcBattleComponent();
	virtual ~UNpcBattleComponent() override = default;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/**
	 * @brief In dependence on the AttackLogic, it can be used to attack.
	 * @see UNpcAttackLogicBase::WantsTick
	 * 
	 * @todo AttackLogick should use delegates instead of ticking, but for now, this is a simple solution.
	 */
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	virtual void BeginPlay() override;

	/**	 
	 * @todo Check how it works without bIsAttacking in the ANpcBase	 
	 */
	void SetIsAttacking(bool bNewIsAttacking);
	void HandleAttackAutoStop(ANpcBase* NpcOwner, const float CurrentTime);


						/* === C++ member variables === */
private:	
	float NextAttackAllowedTime{0.f};
	FTimerHandle AttackAutoStopTimerHandle{};

						/* === Unreal Engine UFUNCTION === */
public:
	/**
	 * @brief Shoul be called on the server. 
	 * 
	 * It is called from AnimNotify.
	 * @see TickComponent
	 * @see UNpcAttackLogicBase::OnDamageWindowStarted
	 * @see UNpcAttackLogicWeaponSweep::TickDamageWindow
	 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void BeginDamageWindow();

	/**
	 * @brief Shoul be called on the server.
	 *
	 * It is called from AnimNotify.
	 * @see UNpcAttackLogicBase::OnDamageWindowEnded
	 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void EndDamageWindow();

	/**
	 * @brief Shoul be called on the server.
	 * 
	 * It is called from StateTree task.
	 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void StartAttack();

	/**
	 * @brief Shoul be called on the server.
	 *
	 * It is called from the NPC's Blueprint once animations have finished playing.
	 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void StopAttack(FString Caller);

	UFUNCTION(BlueprintPure)
	bool IsAttacking() const;

	UFUNCTION(BlueprintPure)
	bool IsTargetInAttackRange(const AActor* Target) const;

	UFUNCTION(BlueprintPure)
	float GetAttackRange() const { return AttackRange; }

	UFUNCTION(BlueprintPure)
	float GetDamageAmount() const { return Damage; }
	
	UFUNCTION(BlueprintAuthorityOnly)
	void SetDamageAmount(float NewDamage);

	UFUNCTION(BlueprintPure)
	TSubclassOf<UDamageType> GetDamageTypeClass() const { return DamageTypeClass; }

	UFUNCTION(BlueprintPure)
	UNpcAttackLogicBase* GetAttackLogicInstance() const { return AttackLogicInstance; }

protected:
	/**
	 * @brief In the current simple implementation, it works on the server.
	 * @todo Animations shouldn't be played on the server.
	 */
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayAttack();


						/* === Unreal Engine UPROPERTY === */
protected:
	/**
	 * @brief The amount of damage the NPC's attack will deal.
	 */
	UPROPERTY(EditDefaultsOnly, Replicated, BlueprintReadOnly)
	float Damage{10.f};

	/**
	 * @brief Indicates whether the NPC is currently performing an attack.
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Replicated)
	bool bIsAttacking{false};

	/**
	 * @brief The type of damage the NPC's attack will deal.
	 * @see UWspDamageType
	 * 
	 * @see It should be as weapon's property. But NPCs have different weapon logic (for example, skeletal mesh with own animation or static mesh with npc's socket) 
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<UDamageType> DamageTypeClass{};

	/**
	 * @brief The attack logic class that defines the behavior of the NPC's weapon.
	 * @see UNpcAttackLogicBase
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UNpcAttackLogicBase> AttackLogic{};

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UNpcAttackLogicBase> AttackLogicInstance{nullptr};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float AttackRange{150.f};	

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	bool bDamageWindowActive{false};

	/**
	 * @brief Used to prevent the NPC from attacking too frequently. It defines the minimum time between consecutive attacks.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = "0.05"))
	float AttackCooldown{1.5f};

	/**
	 * @brief Used to automatically stop the attack after a certain duration.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = "0.05"))
	float AttackMaxDuration{0.6f};
};
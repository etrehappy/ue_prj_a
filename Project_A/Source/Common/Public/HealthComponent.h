/*****************************************************************//**
 * \file   HealthComponent.h
 * \brief  General health component for all actors
 * 
 * \date   January 2026
 *********************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StatusEffect/StatusEffectStatHandler.h"

#include "HealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeathEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnIncreaseHealthEvent, float, HealAmount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDecreaseHealthEvent, float, DamageAmount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChangedEvent, float, CurrentHealthValue, float, MaxHealthValue);


/**
 * @class UHealthComponent
 * @brief Used by player character and enemy actors to manage health
 * @details Responsibilities:
 * - 
 *
 * Network: available
 */
UCLASS( ClassGroup=(CustomHealth), meta=(BlueprintSpawnableComponent) )
class COMMON_API UHealthComponent : public UActorComponent, public IStatusEffectStatHandler
{
	GENERATED_BODY()
						/* === C++ member functions === */
public:	
	UHealthComponent();
	virtual ~UHealthComponent() override = default;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	FORCEINLINE float GetMaxHealth() const;
	FORCEINLINE float GetCurrentHealth() const;
	void SetMaxHealth(float NewHealth);

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


						/* === C++ member variables === */
private:
	void Initialise();
	/**
	 * @brief Server function.
	 * 
	 * @param[in] DamageAmount
	 */
	void DecreaseHealth(float DamageAmount);

	/**
	 * @brief Server function.
	 *
	 * @param[in] HealAmount
	 */
	void IncreaseHealth(float HealAmount);

	/**
	 * @brief Server function.
	 */
	void ToKill();

	/**
	 * @brief Server function.
	 */
	void SetCurrentHealth(float NewHealth);



						/* === Unreal Engine UFUNCTION === */
private:
	/**
	 * @brief Server function.
	 */
	UFUNCTION()
	void TakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);

	UFUNCTION()
	void OnRep_CurrentHealth();

	UFUNCTION()
	void OnRep_MaxHealth();


						/* === Unreal Engine UPROPERTY === */
public:
	UPROPERTY(BlueprintAssignable, Category = "Health")
	FOnDeathEvent OnDeath{};

	UPROPERTY(BlueprintAssignable, Category = "Health")
	FOnIncreaseHealthEvent OnIncreaseHealth{};

	UPROPERTY(BlueprintAssignable, Category = "Health")
	FOnDecreaseHealthEvent OnDecreaseHealth{};

	UPROPERTY(BlueprintAssignable, Category = "Health")
	FOnHealthChangedEvent OnHealthChanged{};

protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Health")
	TObjectPtr<AActor> OwnerActor{nullptr};

	/**
	 * @brief Tags of stats that this component can handle. Should be set up in the Blueprint before the game is run.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "StatusEffect")
	FGameplayTagContainer SupportedStatTags{};

private:
	UPROPERTY(EditDefaultsOnly, ReplicatedUsing = OnRep_MaxHealth, Category = "Health")
	float MaxHealth{100.f};

	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_CurrentHealth, Category = "Health")
	float CurrentHealth{1.f};

	/**
	 * @see UHealthComponent::TakeAnyDamage
	 */
	UPROPERTY()
	float IncomingDamageMultiplier{1.f};

	/**
	 * @brief Value that reduces incoming damage.
	 */
	UPROPERTY()
	float Defense{0.f};
	
};

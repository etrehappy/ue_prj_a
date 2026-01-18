/*****************************************************************//**
 * \file   HealthComponent.h
 * \brief  General health component for all actors
 * 
 * \date   January 2026
 *********************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeathEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnIncreaseHealthEvent, float, HealAmount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDecreaseHealthEvent, float, DamageAmount);

/**
 * @class UHealthComponent
 * @brief Used by player character and enemy actors to manage health
 * @details Responsibilities:
 * - 
 *
 * Network: available
 */
UCLASS( ClassGroup=(CustomHealth), meta=(BlueprintSpawnableComponent) )
class COMMON_API UHealthComponent : public UActorComponent
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

protected:
	virtual void BeginPlay() override;


						/* === C++ member variables === */
private:
	void Initialize();
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
	 * 
	 */
	void ToKill();



						/* === Unreal Engine UFUNCTION === */
private:
	/**
	 * @brief Server function.
	 */
	UFUNCTION()
	void TakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);



						/* === Unreal Engine UPROPERTY === */
public:
	UPROPERTY(BlueprintAssignable, Category = "Health")
	FOnDeathEvent OnDeath{};

	UPROPERTY(BlueprintAssignable, Category = "Health")
	FOnIncreaseHealthEvent OnIncreaseHealth{};

	UPROPERTY(BlueprintAssignable, Category = "Health")
	FOnDecreaseHealthEvent OnDecreaseHealth{};

protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Health")
	TObjectPtr<AActor> OwnerActor{nullptr};

private:
	UPROPERTY(EditDefaultsOnly, Replicated, Category = "Health")
	float MaxHealth{100.f};

	UPROPERTY(VisibleAnywhere, Replicated, Category = "Health")
	float CurrentHealth{};
	




		
	
};

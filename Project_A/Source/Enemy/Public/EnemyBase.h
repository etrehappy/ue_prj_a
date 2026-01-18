

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Actor.h"
#include "BasePawn.h"
#include "EnemyRow.h"
#include "HealthComponent.h"
#include "EnemyBase.generated.h"

UCLASS()
class ENEMY_API AEnemyBase : public ABasePawn
{
	GENERATED_BODY()

						/* === C++ member functions === */
public:	
	AEnemyBase();
	virtual void Tick(float DeltaTime) override;
	void InitializeFromTableData(const FEnemyRow& Data);
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void BeginPlay() override;

private:
	/** Called on server to actually destroy enemy after death FX are started. */
	void DestroyEnemy();
	
	/** Time in seconds to keep the dead actor in the world so clients can see death FX. */
	UPROPERTY(EditDefaultsOnly, Category = "Death")
	float DeathLifeSpan{5.0f};
	
	FTimerHandle DeathTimerHandle{};


						/* === Unreal Engine UFUNCTION === */
protected:
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


						/* === Unreal Engine UPROPERTY === */
private:

	UPROPERTY(EditDefaultsOnly, Replicated, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float Damage{10};

	UPROPERTY(EditDefaultsOnly, Replicated, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float MoveSpeed{300};
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UHealthComponent> HealthComponent{};
};

/*****************************************************************//**
 * \file   AreaExplosion.h
 * \brief  Area explosion actor that handles damage and visual effects.
 * 
 * \date   January 2026
 *********************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WspDamageType.h"

#include "AreaExplosion.generated.h"

/**
 * @class AAreaExplosion
 * @brief It has simple implementation for debugging area explosion effects.
 */
UCLASS()
class WEAPONSYSTEMPLUGIN_API AAreaExplosion : public AActor
{
	GENERATED_BODY()

						/* === C++ member functions === */
public:	
	AAreaExplosion();
	virtual ~AAreaExplosion() = default;
	virtual void Tick(float DeltaTime) override;

	void Initialize(float InRadius);
	void Explode();

protected:
	virtual void BeginPlay() override;	

						/* === Unreal Engine UFUNCTION === */
protected:
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayEffects();

						/* === Unreal Engine UPROPERTY === */
protected:
	UPROPERTY(VisibleAnywhere)
	UParticleSystemComponent* ParticleComponent;

	UPROPERTY(EditAnywhere, Category = "Explosion")
	float Damage{150.f};

	/**
	 * @todo Delete from Initialize 
	 */
	UPROPERTY(EditAnywhere, Category = "Explosion")
	float Radius{};

	UPROPERTY(EditAnywhere, Category = "Explosion")
	TSubclassOf<UWspDamageType> DamageType;

	

};

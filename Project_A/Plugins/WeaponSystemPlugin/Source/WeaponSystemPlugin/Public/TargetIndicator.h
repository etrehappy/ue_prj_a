/*****************************************************************//**
 * \file   TargetIndicator.h
 * \brief  Indicator is needed to visualize the ballistic arc of throwable weapons and the area of effect upon impact.
 * 
 * \date   January 2026
 *********************************************************************/
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Components/DecalComponent.h"
#include "TargetIndicator.generated.h"

/**
 * @class ATargetIndicator
 * @brief The current implementation is used for debugging and visualization of throwable weapon mechanics.
 * @see AWeaponThrowable
 */
UCLASS()
class WEAPONSYSTEMPLUGIN_API ATargetIndicator : public AActor
{
	GENERATED_BODY()

						/** === C++ member functions === */
public:
	ATargetIndicator();
	virtual ~ATargetIndicator() = default;
	virtual void Tick(float DeltaTime) override;

	/**
	 * @brief Client function.
	 * 
	 * ATargetIndicator's general function. On tick, this updates the ballistic arc and the area of effect decal, based on the provided parameters.
	 * @see AWeaponThrowable::UpdateIndicator()
	 */
	void UpdateIndicator(const FVector& StartLocation, const FVector& InitialVelocity, 
		float GravityZ, float ExplosionRadius, int32 MaxSteps = 50, float TimeStep = 0.1f);

	/**
	 * @brief Client function.
	 */
	void Show();

	/**
	 * @brief Client function.
	 */
	void Hide();

	/**
	 * @brief Client function.
	 */
	const FVector& GetFinalHitLocation() const { return FinalHitLocation; }

	/**
	 * @brief Client function.
	 */
	const FVector& GetFinalHitNormal() const { return FinalHitNormal; }

protected:
	virtual void BeginPlay() override;

private:
	/**
	 * @brief Client support function.
	 * 
	 * Recalculate the ballistic arc based on the given parameters.
	 */
	void ComputeArc(const FVector& StartLocation, const FVector& InitialVelocity,
		float GravityZ, int32 MaxSteps, float TimeStep);

	/**
	 * @brief Client support function.
	 * 
	 * Updates spline mesh components to match the computed arc points.
	 */
	void UpdateSplineMeshes();

	/**
	 * @brief Client support function.
	 */
	void UpdateExplosionDecal(float ExplosionRadius);


						/** === C++ member variables === */
protected:
	TArray<FVector> ArcPoints{};
	FVector FinalHitLocation{};
	FVector FinalHitNormal{};

protected:
	UPROPERTY(VisibleAnywhere)
	USplineComponent* ArcSpline{};

	UPROPERTY()
	TArray<USplineMeshComponent*> ArcMeshes{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Indicator")
	UStaticMesh* SplineMesh{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Indicator")
	UMaterialInterface* SplineMaterial{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Indicator")
	UDecalComponent* ExplosionDecal{};


};

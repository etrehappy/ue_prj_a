/*****************************************************************//**
 * \file   ItemPickup.cpp
 * \brief
 *
 * \date   January 2026
 *********************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemPickup.generated.h"

class USphereComponent;

UCLASS(Abstract)
class PROJECT_A_API AItemPickup : public AActor
{
	GENERATED_BODY()
	
public:	
	AItemPickup();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable)
	void PickUp(AActor* Picker);

private:

	UFUNCTION()
	void OtherBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OtherEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

protected:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USphereComponent> SphereComponent{};

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UStaticMeshComponent>StaticMeshComponent{};

};

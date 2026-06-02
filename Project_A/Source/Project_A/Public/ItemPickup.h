/*****************************************************************//**
 * \file   ItemPickup.cpp
 * \brief
 *
 * \date   January 2026
 *********************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactable.h"
#include "InventoryItem.h"

#include "ItemPickup.generated.h"

class USphereComponent;


UCLASS(Abstract)
class PROJECT_A_API AItemPickup : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:	
	AItemPickup();
	virtual void Tick(float DeltaTime) override;

	virtual bool CanInteract(APawn* Interactor) const override;

	//virtual void Interact(APawn* Interactor) override;

	UInventoryItemDefinition* GetItemDefinition() const { return ItemDefinition; }

	virtual void BuildInteractionActions(APawn* Interactor, TArray<FInteractionActionType>& OutActions) const override;
	virtual bool ExecuteInteractionAction(APawn* Interactor, FGameplayTag ActionTag) override;

protected:
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable)
	void PickUp(APawn* Picker);

protected:
	UFUNCTION(BlueprintImplementableEvent, BlueprintAuthorityOnly, Category = "Interaction")
	bool BP_ExecuteInteractionAction(APawn* Interactor, FGameplayTag ActionTag);

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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	TObjectPtr<UInventoryItemDefinition> ItemDefinition;

	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	TArray<TObjectPtr<UInteractionActionDefinition>> InteractionActions{};
};

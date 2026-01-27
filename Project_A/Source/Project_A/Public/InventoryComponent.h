/*****************************************************************//**
 * \file   InventoryComponent.h
 * \brief  
 * 
 * \date   January 2026
 *********************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Inventory.h"

#include "InventoryComponent.generated.h"

/**
 * @class UInventoryComponent
 * @brief It's a temporary solution. The implementation is not yet ready. 
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECT_A_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInventoryComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	virtual void BeginPlay() override;

private:	
	void Initialise();
	
public:
	UFUNCTION(Blueprintable)
	bool AddToInventory(AActor* Item);

protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Inventory")
	TObjectPtr<UInventory> Inventory{};
};

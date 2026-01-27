/*****************************************************************//**
 * \file   Inventory.h
 * \brief  
 * 
 * \date   January 2026
 *********************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "Inventory.generated.h"

/**
 * @class UInventory
 * @brief It's a temporary solution. The implementation is not yet ready. 
 */
UCLASS()
class PROJECT_A_API UInventory : public UObject
{
	GENERATED_BODY()

public:
	UInventory();

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool AddItem(AActor* Item);

protected:
	/**
	 * @brief It's a temporary solution.
	 */
	UPROPERTY()
	TArray<TObjectPtr<AActor>> Container{};

};

//UFUNCTION(BlueprintPure, Category = "Inventory")
//const TArray<AActor*> GetItems() const { return Container; }
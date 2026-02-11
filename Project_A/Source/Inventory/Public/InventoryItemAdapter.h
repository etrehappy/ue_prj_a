
/*****************************************************************//**
 * \file   InventoryItemAdapter.h
 * \brief  Adapter classes for inventory items.
 * 
 * \date   February 2026
 *********************************************************************/
#pragma once

#include "CoreMinimal.h"
#include "InventoryItem.h"

#include "InventoryItemAdapter.generated.h"

class UWeaponDataAsset;

/**
 * @class UInventoryItemAdapter_Weapon
 * @brief An adapter class that extends UInventoryItemDefinition to include weapon-specific data.
 */
UCLASS()
class INVENTORY_API UInventoryItemAdapter_Weapon : public UInventoryItemDefinition
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UWeaponDataAsset> WeaponData{};
};
/*****************************************************************//**
 * \file   InventoryItem.h
 * \brief  It containes UInventoryItem, UInventoryItemDefinition and UInventoryItemAdapter_Weapon class definitions.
 * 
 * \date   February 2026
 *********************************************************************/
#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

#include "InventoryItem.generated.h"

class UTexture2D;
class UInventoryItemDefinition;

/**
 * @class UInventoryItem
 * @brief Represents an item in the inventory.
 * 
 * @details Network: Not supported 
 * @see UInventoryItemDefinition
 * @see FInventorySlot
 * @todo In the current implementation, UInventoryItem is equal to FInventoryReplicatedSlot. But in the future, we may want to add more properties to UInventoryItem that are not needed for replication.
 */
UCLASS(Blueprintable)
class INVENTORY_API UInventoryItem : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	bool HasValidData() const;

	UFUNCTION(BlueprintCallable)
	FGameplayTag GetItemType() const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<UInventoryItemDefinition> Definition{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = "1", ClampMax = "250"))
	int32 StackCount{1};
};

/**
 * @class UInventoryItemDefinition
 * @brief Defines the properties of an inventory item, such as its display name, icon, and type.
 * @see UInventoryItem
 */
UCLASS()
class INVENTORY_API UInventoryItemDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;

	UPROPERTY(EditDefaultsOnly)
	FText DisplayName{};

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UTexture2D> Icon{};

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag ItemType{};

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	bool bConsumable{false};

	/**
	 * @see FStatusEffectDef
	 * @see UStatusEffectComponent
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Effects", meta = (EditCondition = "bConsumable"))
	FGameplayTag EffectTagOnUse{};
};
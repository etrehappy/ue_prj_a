/*****************************************************************//**
 * \file   InventorySlot.h
 * \brief  It contains FInventorySlot, FInventoryReplicatedSlot and FReplicatedSlotArray struct definitions.
 * 
 * \date   February 2026
 *********************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "InventoryItem.h"
#include "Net/Serialization/FastArraySerializer.h"

#include "InventorySlot.generated.h"

class UInventory;

/**
 * @struct FInventorySlot
 * @brief Local runtime representation of an inventory slot.
 * 
 * @details Network: Not supported
 * @see UInventoryItem
 * @todo Is it enough FInventoryReplicatedSlot only? Should we remove FInventorySlot?
 */
USTRUCT(BlueprintType)
struct INVENTORY_API FInventorySlot
{
	GENERATED_BODY()

public:
	bool IsEmpty() const ;
	void Clear();

	UPROPERTY()
	TObjectPtr<UInventoryItem> Item{nullptr};
};




/**
 * @struct FInventoryReplicatedSlot
 * @brief Lightweight version of inventory slot data for network synchronization.
 * @details Network: Supported
 */
USTRUCT(BlueprintType)
struct INVENTORY_API FInventoryReplicatedSlot : public FFastArraySerializerItem
{
	GENERATED_BODY()

public:
	FInventoryReplicatedSlot() = default;
	FInventoryReplicatedSlot(const FInventoryReplicatedSlot& Other);

	bool IsEmpty() const;
	void Clear();
	void CopyPayloadFrom(const FInventoryReplicatedSlot& Other);

	UPROPERTY()
	FPrimaryAssetId DefinitionId{};

	UPROPERTY()
	FSoftObjectPath DefinitionPath{};

	UPROPERTY()
	int32 CurrentStackCount{1};
};




/**
 * @struct FReplicatedSlotArray
 * @brief Container for replicated inventory slots using fast array serialization.
 */
USTRUCT()
struct INVENTORY_API FReplicatedSlotArray : public FFastArraySerializer
{
	GENERATED_BODY()

						/* === C++ member functions === */
public:
	FReplicatedSlotArray() = default;
	~FReplicatedSlotArray() = default;

	/**
	 * @brief Server function.
	 * 
	 * Adds new slot data to a specific slot index.
	 * @param[in] NewSlotData The slot data to add.
	 * @param[in] SlotIndex The index at which to add the slot data.
	 * @return true if the slot data was added successfully; otherwise, false.
	 */
	bool AddToSlot(const FInventoryReplicatedSlot& NewSlotData, int32 SlotIndex);

	/**
	 * @brief Server function.
	 * 
	 * Adds a new slot or stacks it if possible.
	 * @param[in] NewSlot The slot to add or stack.
	 * @return true if the slot was added or stacked successfully; otherwise, false.
	 * @see Stack
	 * @todo Stack is off 
	 */
	bool AddOrStack(const FInventoryReplicatedSlot& NewSlot);

	/**
	 * @brief Handles the delta serialization for network replication. FFastArraySerializer uses it. 
	 * 
	 * @param[in] DeltaParms Information about the delta serialization process.
	 * @return true if the serialization was successful, false otherwise.
	 */
	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms);

	/**
	 * @brief Client function.
	 * 
	 * FFastArraySerializer uses it for new slots.	 
	 */
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 Staleness);

	/**
	 * @brief Client function.
	 * 
	 * FFastArraySerializer uses it for changed slots.	 
	 */
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 Staleness);

	/**
	 * @brief Client function.
	 * 
	 * FFastArraySerializer uses it for cleaned slots.	 
	 */
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 Staleness);

	void SetOwner(UInventory* InOwner);

	/**
	 * @brief Sets the number of replicated slots.
	 * 
	 * @param[in] NewNum The new number of slots.
	 */
	void SetNum(const int32 NewNum);

private:
	/**
	 * @brief Server function.	 
	 * @return true if the slot was stacked successfully; otherwise, false.
	 * @see AddOrStack
	 * @todo Add visualisation for stackable slots
	 */
	bool Stack(const FInventoryReplicatedSlot& NewSlot);

	/**
	 * @brief Server function.	 
	 * @return true if the slot was added successfully; otherwise, false.
	 */
	bool Add(const FInventoryReplicatedSlot& NewSlot);


						/* === Unreal Engine UPROPERTY === */
public:
	/**
	 * @brief Array of replicated inventory slots.
	 */
	UPROPERTY()
	TArray<FInventoryReplicatedSlot> Items{};

private:
	/**
	 * @brief An inventory that owns this FReplicatedSlotArray
	 */
	UPROPERTY()
	TWeakObjectPtr<UInventory> Owner{};
};


/**
 * @brief Enables NetDeltaSerializer for FReplicatedSlotArray to allow network replication.
 */
template<>
struct TStructOpsTypeTraits<FReplicatedSlotArray> : public TStructOpsTypeTraitsBase2<FReplicatedSlotArray>
{
	enum { WithNetDeltaSerializer = true };
};

/*****************************************************************//**
 * \file   Inventory.h
 * \brief  General inventory system for managing items and slots.
 * 
 * \date   January 2026
 *********************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "InventorySlot.h"

#include "Inventory.generated.h"

class UInventory;
class UInventoryItem;
class UInventoryItemDefinition;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnInventoryMoveRequested,
	UInventory*, SourceInventory,
	UInventory*, TargetInventory,
	int32, SourceSlotIndex,
	int32, TargetSlotIndex);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnInventoryRelocateRequested,
	UInventory*, Inventory,
	int32, FromIndex,
	int32, ToIndex);

/**
 * @class UInventory
 * @brief It supports adding and relocating items within the inventory, as well as moving items between different inventories. 
 * 
 * @details Responsibilities:
 * - Manage inventory slots and their replication to clients.
 * - Handle item addition, relocation, and movement between inventories.
 * - Broadcast inventory change events to update UI and other systems. 
 * 
 * Network: Supported (replication of inventory slots to clients).
 */
UCLASS(Blueprintable)
class INVENTORY_API UInventory : public UObject
{
	GENERATED_BODY()

						/* === C++ member functions === */
public:
	UInventory();
	virtual ~UInventory() = default;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/**
	 * @brief It is required for subobject replication and NetGUID.
	 * 
	 * @return true if networking is supported; otherwise, false.
	 */
	virtual bool IsSupportedForNetworking() const override { return true; }
		
	/**
	 * @brief It is called on clients when inventory slots are added via replication.
	 * @see FReplicatedSlotArray::PostReplicatedAdd
	 * 
	 * @param[in] AddedIndices Indices of the added slots.
	 */
	void HandleReplicatedAdd(const TArrayView<int32> AddedIndices);

	/**
	 * @brief It is called on clients when inventory slots are changed via replication.
	 * @see FReplicatedSlotArray::PostReplicatedChange
	 * 
	 * @param[in] ChangedIndices Indices of the changed slots.
	 */
	void HandleReplicatedChange(const TArrayView<int32> ChangedIndices);

	/**
	 * @brief It is called on clients when inventory slots are removed via replication.
	 * @see FReplicatedSlotArray::PostReplicatedRemove
	 * 
	 * @param[in] RemovedIndices Indices of the removed slots.
	 */
	void HandleReplicatedRemove(const TArrayView<int32> RemovedIndices);

	/**
	 * @brief Server function.
	 *
	 * @see RelocateItemInInventory
	 * @see UInventoryComponent::RelocateItemInInventory
	 */
	void RelocateItemInInventoryInternal(int32 FromIndex, int32 ToIndex);

	/**
	 * @brief Server function.
	 *
	 * @see MoveItemToOtherInventory
	 * @see UInventoryComponent::MoveItemToOtherInventory
	 */
	void MoveItemToOtherInventoryInternal(UInventory* TargetInventory, int32 SourceSlotIndex, int32 TargetSlotIndex);

	/**
	 * @brief Uses an item from the specified slot.
	 * @see UInventoryComponent::UseItem
	 */
	void UseItemFromSlot(int32 SlotIndex);

private:
	void UpdateInventorySize(int32 NewSize);
	/**
	 * @brief Helper function.	 
	 */
	void EnsureLocalSlotsMatchReplicated();

	/**
	 * @brief Server and client function.
	 * 
	 * Updates a local inventory slot based on the corresponding replicated slot data.	 
	 * @param[in] Index The index of the slot to update.
	 * @return true if the slot was successfully updated; otherwise, false.
	 */
	bool UpdateLocalSlotFromReplicated(const int32 Index);

	/**
	 * @brief Server function.
	 * 
	 * It adds an item to the replicated inventory slots.
	 * @see ReplicatedSlots	 
	 */
	bool AddItemToReplicatedSlots(const UInventoryItem* Item);

	/**
	 * @brief It adds an item to the local inventory slots. It is used for server-side inventory management and client-side prediction.
	 * @see InventorySlots
	 */
	bool AddItemToLocalInventory(const UInventoryItem* Item);

	/**
	 * @brief Server function.
	 * 
	 * It adds an item to a specific slot in the replicated inventory slots.	 
	 * @return true if the item was added successfully; otherwise, false.
	 */
	bool AddItemToReplicatedSlot(const UInventoryItem* Item, int32 SlotIndex);

	/**
	 * @brief Server function.
	 * 
	 * It adds an item to a specific slot in the local inventory slots.
	 * @return true if the item was added successfully; otherwise, false.
	 */
	bool AddItemToLocalInventory(const UInventoryItem* Item, int32 SlotIndex);

	/**
	 * @brief Helper function.
	 * @see UpdateLocalSlotFromReplicated
	 * @todo consider async loading for better performance 
	 */
	UInventoryItemDefinition* LoadItemDefinitionFromReplicatedSlot(const FInventoryReplicatedSlot& ReplicatedSlot) const;

	/**
	 * @brief Helper function.
	 * 
	 * Copies the runtime UInventoryItem instances from one local inventory slot to another.
	 * @see RelocateItemInInventoryInternal
	 */
	void CopyLocalRuntimeItem(int32 FromIndex, int32 ToIndex);

	/**
	 * @brief Helper function.
	 * 
	 * Moves the runtime UInventoryItem instance from one local inventory slot to another, leaving the source slot empty.
	 * @see MoveItemToOtherInventoryInternal
	 */
	void MoveLocalRuntimeItemToInventory(UInventory* TargetInventory, int32 FromIndex, int32 ToIndex);
	
	/**
	 * @brief Checks if two items can be stacked together based on their definitions.
	 */
	bool IsStackable(const UInventoryItemDefinition* SourceItem, const UInventoryItemDefinition* TargetItem) const;

	/**
	 * @brief Gets the maximum stack count for a given item definition. If the item is not stackable, it returns 1.
	 */
	int32 GetMaxStackCount(const UInventoryItemDefinition* Definition) const;

	/**
	 * @brief Server function.
	 * 
	 * Attempts to stack an item into a specific slot in the inventory. If the slot contains a stackable item of the same type, it will increase the stack count up to the maximum allowed. 
	 * 
	 * @param[in] Item The item to stack.
	 * @param[in] SlotIndex The index of the slot to stack the item into.
	 * @return true if the item was successfully stacked; otherwise, false.
	 */
	bool TryStackItemIntoSlot(const UInventoryItem* Item, int32 SlotIndex);

	/**
	 * @brief Server function.
	 * 
	 * Attempts to stack an item from a source inventory slot into a target inventory slot. If the target slot contains a stackable item of the same type, it will increase the stack count up to the maximum allowed, and decrease the source stack count accordingly.
	 * 
	 * @param[in] SourceInventory The inventory containing the source item.
	 * @param[in] SourceSlotIndex The index of the source slot.
	 * @param[in] TargetInventory The inventory containing the target slot.
	 * @param[in] TargetSlotIndex The index of the target slot.
	 * @return true if the item was successfully stacked; otherwise, false.
	 */
	bool TryStackSourceIntoTarget(UInventory* SourceInventory, int32 SourceSlotIndex, UInventory* TargetInventory, int32 TargetSlotIndex);

	/**
	 * @brief Updates a local inventory slot based on the corresponding replicated slot data. It is used to keep the local inventory state in sync with the replicated data received from the server.
	 * 
	 * @param[in] SlotIndex The index of the slot to update.
	 */
	void RefreshLocalSlotFromReplicated(int32 SlotIndex);

						/* === Unreal Engine UFUNCTION === */
public:
	/**
	 * @brief Should be called after creating an inventory object to initialise the number of slots.
	 *
	 * @param[in] InSize The number of inventory slots to initialise.
	 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Inventory")
	void Initialise(int32 InSize);

	/**
	 * @brief Server function.
	 * 
	 * Adds an item to the inventory. If there is an empty slot or a stackable slot, the item will be added or stacked.
	 * @param[in] Item The item to add to the inventory.
	 * @return true if the item was added successfully; otherwise, false.
	 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Inventory")
	bool AddItem(const UInventoryItem* Item);

	/**
	 * @brief Server function.
	 * 
	 * Adds an item to a specific slot in the inventory.	  
	 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Inventory")
	bool AddItemToSlot(const UInventoryItem* Item, int32 SlotIndex);

	/**
	 * @brief Finds an item in the inventory by its slot index. 
	 * 	 
	 * @return A pointer to the item in the specified slot, or nullptr if the slot is empty or the index is invalid.
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	UInventoryItem* FindItemBySlot(int32 SlotIndex) const;

	/**	 
	 * @return The total number of inventory slots.
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	int32 GetSize() const { return InventorySlots.Num(); };

	/**
	 * @return The number of replicated inventory slots.
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	int32 GetReplicatedSize() const { return ReplicatedSlots.Items.Num(); };

	/**
	 * @return The array of replicated inventory slots.
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	const TArray<FInventoryReplicatedSlot>& GetReplicatedSlots() const { return ReplicatedSlots.Items; };

	/**
	 * @return The array of inventory slots.
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	const TArray<FInventorySlot>& GetSlots() const {return InventorySlots; };

	/**
	 * @brief Client function.
	 *	 
	 * @see OnRelocateItemRequested
	 * @see RelocateItemInInventoryInternal	  
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void RelocateItemInInventory(int32 FromIndex, int32 ToIndex);


	/**
	 * @brief Client function.
	 * 
	 * @see OnMoveItemRequested
	 * @see MoveItemToOtherInventoryInternal
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void MoveItemToOtherInventory(int32 FromIndex, UInventory* TargetInventory, int32 TargetIndex);


						/* === Unreal Engine UPROPERTY === */
public:
	/**
	 * @brief Delegate that is broadcasted whenever the inventory changes (items added, removed, or modified).
	 * @see UInventoryWidget
	 * @todo There is HandleReplicatedAdd. Should this be deleted?
	 */
	UPROPERTY()
	FOnInventoryChanged OnInventoryChanged{};

	/**
	 * @brief whenever an item is moved to other inventory.
	 */
	UPROPERTY()
	FOnInventoryMoveRequested OnMoveItemRequested{};

	/**
	 * @brief whenever an item is relocated within the same inventory.
	 */
	UPROPERTY()
	FOnInventoryRelocateRequested OnRelocateItemRequested{};

protected:
	/**
	 * @brief It is used to store the actual inventory slots on the server and clients. It is not replicated directly, but is restored from the ReplicatedSlots (FFastArray).
	 */
	UPROPERTY(EditDefaultsOnly)
	TArray<FInventorySlot> InventorySlots{};

	/**
	 * @brief This array is replicated to clients and contains the data for the inventory slots. It uses fast array serialization for network replication.
	 * @see FInventoryReplicatedSlot, FReplicatedSlotArray
	 */
	UPROPERTY(Replicated)
	FReplicatedSlotArray ReplicatedSlots{};
};

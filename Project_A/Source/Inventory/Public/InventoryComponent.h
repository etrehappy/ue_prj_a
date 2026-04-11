/*****************************************************************//**
 * \file   InventoryComponent.h
 * \brief  Inventory component for managing items and equipment.
 *
 * \date   February 2026
 *********************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "InventoryComponent.generated.h"

class UInventory;
class UInventoryItem;
class UActorChannel;
class FOutBunch;
struct FReplicationFlags;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEquipmentChanged);

/**
 * @enum EEquipmentSlot
 * @brief Temporary solution for equipment slots.
 * @todo Should be separated calss
 */
UENUM(BlueprintType)
enum class EEquipmentSlot : uint8
{
	None		= 0	UMETA(Hidden)

	, Weapon	= 1	UMETA(DisplayName = "Weapon")
	, Throwable	= 2	UMETA(DisplayName = "Throwable")

	, Max			UMETA(Hidden)
};


/**
 * @class UInventoryComponent
 * @brief Actor component that owns inventory and equipment state.
 *
 * Network: Supported (inventory subobject replication).
 */
UCLASS(ClassGroup = (CustomInventory), meta = (BlueprintSpawnableComponent))
class INVENTORY_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

						/* === C++ member functions === */
public:
	UInventoryComponent();

	virtual void BeginPlay() override;
	/*virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;*/
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/**
	 * @brief It is required for replicating inventory subobjects to client.
	 * @see Inventory, EquipmentInventory
	 * @see UInventory
	 */
	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;

	/**
	 * @brief Adds item to the owned inventory.
	 */
	bool AddToInventory(UInventoryItem* Item);

	/**
	 * @brief Returns the owned inventory object.
	 */
	UInventory* GetInventory() const { return Inventory; }

	/**
	 * @brief Returns the owned equipment inventory object.
	 */
	UInventory* GetEquipmentInventory() const { return EquipmentInventory; }

	/**
	 * @brief Unequips the item from a slot.
	 * @todo Not ready. Should be implemented
	 */
	void UnequipItem(EEquipmentSlot Slot);

	/**
	 * @brief Returns the inventory index corresponding to the equipment slot, or INDEX_NONE if the slot is invalid or empty.
	 */
	int32 GetEquipmentInventoryIndex(EEquipmentSlot Slot) const;

private:
	/**
	 * @brief Helper function.
	 * 
	 * Creates inventory and equipment inventory if necessary.
	 * @see BeginPlay
	 */
	void Initialise();

	/**
	 * @brief Checks the slots available in EEquipmentSlot and fills them with data.
	 * @see EquipmentInventory
	 */
	void FillEquipmentSlots();

	/**
	 * @brief Helper function.
	 * @see FillEquipmentSlots
	 */
	int32 GetEquipmentSlotNumbers(const UEnum* EnumPtr) const;

	/**
	 * @brief Binds inventory delegates to handle item movement and relocation requests.
	 * It is required because Inventory is a UObject and does not have a reference to its owning component, so it cannot directly call the component's server functions for moving items.
	 * 
	 * @param[in] InInventory The inventory to bind delegates for.
	 */
	void BindInventoryDelegates(UInventory* InInventory);

	/**
	 * @brief Returns the equipment slot corresponding to the inventory index, or EEquipmentSlot::None if the index is invalid.
	 */
	EEquipmentSlot GetEquipmentSlotFromInventoryIndex(int32 SlotIndex) const;

	/**
	 * @brief Checks if an item can be equipped in a slot based on the item's type and the slot's requirements.
	 * 
	 * @param[in] Slot The equipment slot to check.
	 * @param[in] Item The item to check.
	 * @return true if the item can be equipped in the slot; otherwise, false.
	 */
	bool CanEquipItemInSlot(EEquipmentSlot Slot, const UInventoryItem* Item) const;

	/**
	 * @brief Checks if an item can be placed in an equipment slot based on the item's type and the slot's requirements.
	 * 
	 * @param[in] TargetSlotIndex The index of the equipment slot to check.
	 * @param[in] Item The item to check.
	 * @return true if the item can be placed in the equipment slot; otherwise, false.
	 */
	bool CanPlaceItemIntoEquipmentSlot(int32 TargetSlotIndex, const UInventoryItem* Item) const;

	
						/* === C++ member variables === */
private:	
	/**
	 * @brief The last inventory that had delegates bound.
	 * @see OnRep_Inventory
	 */
	TObjectPtr<UInventory> LastBoundInventory{};

	/**
	 * @brief The last equipment inventory that had delegates bound.
	 * @see OnRep_EquipmentInventory
	 */
	TObjectPtr<UInventory> LastBoundEquipmentInventory{};

	/**
	 * @see Inventory
	 */
	const int32 DefaultInventorySize{5};


						/* === Unreal Engine UFUNCTION === */
public:
	/**
	 * @brief Returns the equipped item in the Slot, if any.
	 */
	UFUNCTION(BlueprintCallable)
	UInventoryItem* GetEquippedItem(EEquipmentSlot Slot) const;

	/**
	 * @brief Server function.
	 * 
	 *  Equips an item into a slot.
	 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void AddItemToEquipment(EEquipmentSlot Slot, UInventoryItem* Item);

	/**
	 * @brief Server function.
	 * 
	 * It applies the item's effect to the owner and removes the item from the inventory. 
	 * @see StatusEffectComponent
	 * @see UInventory::UseItemFromSlot
	 */
	UFUNCTION(BlueprintCallable)
	void UseItem(int32 SlotIndex);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void SetInventory(UInventory* NewInventory);

	/**
	 * @brief If item is equipped in the Slot, it applies the item's effect to the owner and removes the item from the inventory. Otherwise, does nothing.
	 * 
	 * @param[in] Slot The equipment slot to use the item from.
	 */
	UFUNCTION(BlueprintCallable)
	void UseEquippedItem(EEquipmentSlot Slot);
	
protected:
	/**
	 * @brief Only calls MoveItemToOtherInventory 
	 * @see MoveItemToOtherInventory
	 */
	UFUNCTION(Server, Reliable)
	void Server_MoveItemToOtherInventory(UInventory* SourceInventory, UInventory* TargetInventory, int32 SourceSlotIndex, int32 TargetSlotIndex);

	/**
	 * @brief Only calls RelocateItemInInventory 
	 * @see RelocateItemInInventory
	 */
	UFUNCTION(Server, Reliable)
	void Server_RelocateItemInInventory(UInventory* InInventory, int32 FromIndex, int32 ToIndex);

	/**
	 * @brief Only calls UseItem 
	 * @see UseItem
	 */
	UFUNCTION(Server, Reliable)
	void Server_UseItem(int32 SlotIndex);

	UFUNCTION(Server, Reliable)
	void Server_SetInventory(UInventory* NewInventory);

	/**
	 * @brief Only calls UseEquippedItem
	 * @see UseEquippedItem
	 */
	UFUNCTION(Server, Reliable)
	void Server_UseEquippedItem(EEquipmentSlot Slot);

private:
	/**
	 * @brief Calls the inventory's function for moving items between inventories on the server	 
	 */
	UFUNCTION()
	void MoveItemToOtherInventory(UInventory* SourceInventory, UInventory* TargetInventory, int32 SourceSlotIndex, int32 TargetSlotIndex);

	/**
	 * @brief Calls the inventory's function for relocating items within an inventory on the server	 
	 */
	UFUNCTION()
	void RelocateItemInInventory(UInventory* InInventory, int32 FromIndex, int32 ToIndex);

	/**
	 * @brief 
	 * @see Inventory
	 */
	UFUNCTION()
	void OnRep_Inventory();

	/**
	 * @brief 
	 * @see EquipmentInventory
	 */
	UFUNCTION()
	void OnRep_EquipmentInventory();	


						/* === Unreal Engine UPROPERTY === */
public:
	/**
	 * @brief Broadcasted when equipment changes.
	 */
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnEquipmentChanged OnEquipmentChanged{};

private:
	/**
	 * @brief Inventory subobject (replicated as a subobject).
	 */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, ReplicatedUsing = OnRep_Inventory, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInventory> Inventory{};

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, ReplicatedUsing = OnRep_EquipmentInventory, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInventory> EquipmentInventory{};	
};



#include "Inventory.h"

#include "Engine/AssetManager.h" 
#include "InventoryItem.h"
#include "InventorySlot.h"
#include "Net/UnrealNetwork.h"

#include "ProjectALog.h"


UInventory::UInventory()
{
	ReplicatedSlots.SetOwner(this);
}

void UInventory::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UInventory, ReplicatedSlots);
}

void UInventory::Initialise(int32 InSize)
{    
	UpdateInventorySize(InSize);
}

void UInventory::HandleReplicatedAdd(const TArrayView<int32> AddedIndices)
{
	//UE_LOGFMT(LogProjectA, Error, "{0} - called with {1} added indices", FString(__FUNCTION__), AddedIndices.Num());

	EnsureLocalSlotsMatchReplicated();

	bool bAnySlotUpdated = false;

	for (int32 Index : AddedIndices)
	{
		if (!ReplicatedSlots.Items.IsValidIndex(Index) || !InventorySlots.IsValidIndex(Index))
		{
			UE_LOGFMT(LogProjectA, Log, "{0} - Invalid index {1} in AddedIndices", FString(__FUNCTION__), Index);
			continue;
		}

		if (!UpdateLocalSlotFromReplicated(Index))
		{
			continue;
		}

		bAnySlotUpdated = true;
	}

	if (bAnySlotUpdated)
	{
		OnInventoryChanged.Broadcast();
	}
}

void UInventory::HandleReplicatedChange(const TArrayView<int32> ChangedIndices)
{
	//UE_LOGFMT(LogProjectA, Log, "{0} - called with {1} changed indices", FString(__FUNCTION__), ChangedIndices.Num());

	EnsureLocalSlotsMatchReplicated();

	HandleReplicatedAdd(ChangedIndices);
}

void UInventory::HandleReplicatedRemove(const TArrayView<int32> RemovedIndices)
{
	//UE_LOGFMT(LogProjectA, Log, "{0} - called with {1} removed indices", FString(__FUNCTION__), RemovedIndices.Num());

	bool bAnySlotUpdated = false;
	TArray<int32> SortedIndices{};
	SortedIndices.Append(RemovedIndices.GetData(), RemovedIndices.Num());
	SortedIndices.Sort(TGreater<int32>());

	for (int32 Index : SortedIndices)
	{
		if (InventorySlots.IsValidIndex(Index))
		{
			InventorySlots.RemoveAt(Index);
			bAnySlotUpdated = true;	
		}
	}

	if (bAnySlotUpdated)
	{
		OnInventoryChanged.Broadcast();
	}
}

void UInventory::UpdateInventorySize(int32 NewSize)
{
	//UE_LOGFMT(LogProjectA, Log, "{0} - called with NewSize={1}", FString(__FUNCTION__), NewSize);

	if (!IsRunningDedicatedServer())
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Only the server can modify inventory size", FString(__FUNCTION__));
		return;
	}

	InventorySlots.SetNum(NewSize);
	ReplicatedSlots.SetNum(NewSize);

	OnInventoryChanged.Broadcast();
}

void UInventory::EnsureLocalSlotsMatchReplicated()
{
	const int32 Desired = ReplicatedSlots.Items.Num();
	if (InventorySlots.Num() != Desired)
	{
		InventorySlots.SetNum(Desired);
		//UE_LOGFMT(LogProjectA, Log, "{0} - Resized InventorySlots to {1}", FString(__FUNCTION__), InventorySlots.Num());
	}
}

bool UInventory::UpdateLocalSlotFromReplicated(const int32 Index)
{
	const FInventoryReplicatedSlot& RepSlot = ReplicatedSlots.Items[Index];
	FInventorySlot& LocalSlot = InventorySlots[Index];

	if (RepSlot.IsEmpty())
	{
		const bool bWasEmpty = LocalSlot.IsEmpty();
		LocalSlot.Clear();
		return !bWasEmpty; // to clean UI if the slot was not empty before
	}

	if (!LocalSlot.Item)
	{
		LocalSlot.Item = NewObject<UInventoryItem>(this);
	}

	auto* ResolvedDef = LoadItemDefinitionFromReplicatedSlot(RepSlot);
	if(!ResolvedDef)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Failed to resolve definition", FString(__FUNCTION__));
	}

	LocalSlot.Item->Definition = ResolvedDef;
	LocalSlot.Item->StackCount = RepSlot.CurrentStackCount;

	return true;
}

bool UInventory::AddItemToReplicatedSlots(const UInventoryItem* Item)
{
	if (!Item)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Item is null", FString(__FUNCTION__));
		return false;
	}

	if (!Item->HasValidData())
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Item has invalid data", FString(__FUNCTION__));
		return false;
	}

	FInventoryReplicatedSlot NewSlot{};
	NewSlot.DefinitionId = Item->Definition->GetPrimaryAssetId();
	NewSlot.DefinitionPath = FSoftObjectPath(Item->Definition);
	NewSlot.CurrentStackCount = Item->StackCount;

	return ReplicatedSlots.AddOrStack(NewSlot);
}

bool UInventory::AddItemToLocalInventory(const UInventoryItem* Item)
{
	if (!Item)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Item is null", FString(__FUNCTION__));
		return false;
	}
	
	EnsureLocalSlotsMatchReplicated();

	for (int32 Index = 0; Index < ReplicatedSlots.Items.Num(); ++Index)
	{
		if (!UpdateLocalSlotFromReplicated(Index))
		{
			continue;
		}
	}

	return true;
}

bool UInventory::AddItem(const UInventoryItem* Item)
{
    UE_LOGFMT(LogProjectA, Log, "{0} - called", FString(__FUNCTION__));

    if (!Item)
    {
        UE_LOGFMT(LogProjectA, Warning, "{0} - Item is null", FString(__FUNCTION__));
        return false;
    }

	if (!IsRunningDedicatedServer())
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Only the server can modify inventory", FString(__FUNCTION__));
		return false;
	}

	if (!AddItemToReplicatedSlots(Item))
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Failed to add item to replicated slots, inventory might be full", FString(__FUNCTION__));
		return false;
	}

	if (!AddItemToLocalInventory(Item))
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Failed to add item to local inventory", FString(__FUNCTION__));
		return false;
	}

	return true;
}

bool UInventory::AddItemToSlot(const UInventoryItem* Item, int32 SlotIndex)
{
	UE_LOGFMT(LogProjectA, Log, "{0} - called with SlotIndex={1}", FString(__FUNCTION__), SlotIndex);

	if (!IsRunningDedicatedServer())
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Only the server can modify inventory", FString(__FUNCTION__));
		return false;
	}	

	if (!AddItemToReplicatedSlot(Item, SlotIndex))
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Failed to add item to replicated slots, inventory might be full", FString(__FUNCTION__));
		return false;
	}

	if (!AddItemToLocalInventory(Item, SlotIndex))
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Failed to add item to local inventory", FString(__FUNCTION__));
		return false;
	}

	return true;
}

UInventoryItem* UInventory::FindItemBySlot(int32 SlotIndex) const
{
	// UE_LOGFMT(LogProjectA, Log, "{0} - called with SlotIndex={1}", FString(__FUNCTION__), SlotIndex);	

	if (InventorySlots.IsEmpty())
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - InventorySlots is empty", FString(__FUNCTION__));
		return nullptr;		
	}

	if (!InventorySlots.IsValidIndex(SlotIndex))
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Invalid SlotIndex {1}", FString(__FUNCTION__), SlotIndex);
		return nullptr;
	}

	return InventorySlots[SlotIndex].Item;
}

bool UInventory::AddItemToReplicatedSlot(const UInventoryItem* Item, int32 SlotIndex)
{
	//UE_LOGFMT(LogProjectA, Log, "{0} - called with SlotIndex={1}", FString(__FUNCTION__), SlotIndex);

	if (!Item)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Item is null", FString(__FUNCTION__));
		return false;
	}
	
	if (!Item->HasValidData())
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Item has invalid data", FString(__FUNCTION__));
		return false;
	}	

	if (!InventorySlots.IsValidIndex(SlotIndex))
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Invalid SlotIndex {1}", FString(__FUNCTION__), SlotIndex);
		return false;
	}

	if (!InventorySlots[SlotIndex].IsEmpty())
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Slot {1} is not empty", FString(__FUNCTION__), SlotIndex);
		return false;
	}

	FInventoryReplicatedSlot NewSlotData{};	
	NewSlotData.DefinitionId = Item->Definition->GetPrimaryAssetId();
	NewSlotData.DefinitionPath = FSoftObjectPath(Item->Definition);
	NewSlotData.CurrentStackCount = Item->StackCount;

	return ReplicatedSlots.AddToSlot(NewSlotData, SlotIndex);
}

bool UInventory::AddItemToLocalInventory(const UInventoryItem* Item, int32 SlotIndex)
{
	if (!Item)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Item is null", FString(__FUNCTION__));
		return false;
	}
		
	if (!InventorySlots.IsValidIndex(SlotIndex))
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Invalid SlotIndex {1}", FString(__FUNCTION__), SlotIndex);
		return false;
	}

	if (!InventorySlots[SlotIndex].IsEmpty())
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Slot {1} is not empty", FString(__FUNCTION__), SlotIndex);
		return false;
	}

	InventorySlots[SlotIndex].Item = NewObject<UInventoryItem>(this);
	InventorySlots[SlotIndex].Item->Definition = Item->Definition;
	InventorySlots[SlotIndex].Item->StackCount = Item->StackCount;

	return true;
}

UInventoryItemDefinition* UInventory::LoadItemDefinitionFromReplicatedSlot(const FInventoryReplicatedSlot& ReplicatedSlot) const
{
	UInventoryItemDefinition* ResolvedDef = nullptr;
	UAssetManager& AssetManager = UAssetManager::Get();

	// Try to resolve the item definition for the replicated slot using multiple strategies:	
	if (ReplicatedSlot.DefinitionId.IsValid())
	{
		UObject* LoadedById = AssetManager.GetPrimaryAssetObject(ReplicatedSlot.DefinitionId);
		if (LoadedById)
		{
			ResolvedDef = Cast<UInventoryItemDefinition>(LoadedById);
		}
	}

	if (!ResolvedDef && !ReplicatedSlot.DefinitionPath.IsNull())
	{
		UObject* LoadedByPath = ReplicatedSlot.DefinitionPath.TryLoad(); // sync load
		if (LoadedByPath)
		{
			ResolvedDef = Cast<UInventoryItemDefinition>(LoadedByPath);
		}
	}
	
	return ResolvedDef;
}

void UInventory::CopyLocalRuntimeItem(int32 FromIndex, int32 ToIndex)
{
	EnsureLocalSlotsMatchReplicated();

	if (InventorySlots.IsValidIndex(FromIndex) && InventorySlots[FromIndex].Item)
	{
		UInventoryItem* SrcItem = InventorySlots[FromIndex].Item;

		InventorySlots[ToIndex].Item = NewObject<UInventoryItem>(this);
		if (InventorySlots[ToIndex].Item)
		{
			InventorySlots[ToIndex].Item->Definition = SrcItem->Definition;
			InventorySlots[ToIndex].Item->StackCount = SrcItem->StackCount;
		}

		InventorySlots[FromIndex].Item = nullptr;
	}
}

void UInventory::MoveLocalRuntimeItemToInventory(UInventory* TargetInventory, int32 FromIndex, int32 ToIndex)
{	
	if (!TargetInventory)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - TargetInventory is null", FString(__FUNCTION__));
		return;
	}

	EnsureLocalSlotsMatchReplicated();

	if (TargetInventory->InventorySlots.Num() != TargetInventory->ReplicatedSlots.Items.Num())
	{
		TargetInventory->InventorySlots.SetNum(TargetInventory->ReplicatedSlots.Items.Num());
	}

	if (InventorySlots.IsValidIndex(FromIndex) && InventorySlots[FromIndex].Item)
	{
		UInventoryItem* SrcItem = InventorySlots[FromIndex].Item;

		TargetInventory->InventorySlots[ToIndex].Item = NewObject<UInventoryItem>(TargetInventory);
		if (TargetInventory->InventorySlots[ToIndex].Item)
		{
			TargetInventory->InventorySlots[ToIndex].Item->Definition = SrcItem->Definition;
			TargetInventory->InventorySlots[ToIndex].Item->StackCount = SrcItem->StackCount;
		}

		InventorySlots[FromIndex].Item = nullptr;
	}
}

void UInventory::RelocateItemInInventory(int32 FromIndex, int32 ToIndex)
{
	//UE_LOGFMT(LogProjectA, Log, "{0} - called with FromIndex={1}, ToIndex={2}", FString(__FUNCTION__), FromIndex, ToIndex);

	if (!OnRelocateItemRequested.IsBound())
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - No handler bound for OnRelocateItemRequested", FString(__FUNCTION__));
		return;
	}

	OnRelocateItemRequested.Broadcast(this, FromIndex, ToIndex);
}

void UInventory::RelocateItemInInventoryInternal(int32 FromIndex, int32 ToIndex)
{
	//UE_LOGFMT(LogProjectA, Log, "{0} - called with FromIndex={1}, ToIndex={2}", FString(__FUNCTION__), FromIndex, ToIndex);

	// Validation
	if (!IsRunningDedicatedServer())
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Only the server can modify inventory", FString(__FUNCTION__));
		return;
	}

	if (!ReplicatedSlots.Items.IsValidIndex(FromIndex) || !ReplicatedSlots.Items.IsValidIndex(ToIndex))
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Invalid indices FromIndex={1}, ToIndex={2}", FString(__FUNCTION__), FromIndex, ToIndex);
		return;
	}

	if (FromIndex == ToIndex)
	{
		UE_LOGFMT(LogProjectA, Log, "{0} - FromIndex and ToIndex are the same ({1})", FString(__FUNCTION__), FromIndex);
		return;
	}

	FInventoryReplicatedSlot& SourceSlot = ReplicatedSlots.Items[FromIndex];
	FInventoryReplicatedSlot& TargetSlot = ReplicatedSlots.Items[ToIndex];

	if (SourceSlot.IsEmpty())
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Source slot {1} is empty", FString(__FUNCTION__), FromIndex);
		return;
	}

	if (!TargetSlot.IsEmpty())
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Target slot {1} is not empty", FString(__FUNCTION__), ToIndex);
		return;
	}
	
	// Move payload only
	TargetSlot.CopyPayloadFrom(SourceSlot);
	SourceSlot.Clear();

	ReplicatedSlots.MarkItemDirty(TargetSlot);
	ReplicatedSlots.MarkItemDirty(SourceSlot);
	
	CopyLocalRuntimeItem(FromIndex, ToIndex);	

	//OnInventoryChanged.Broadcast();
}

void UInventory::MoveItemToOtherInventory(int32 FromIndex, UInventory* TargetInventory, int32 TargetIndex)
{
	//UE_LOGFMT(LogProjectA, Log, "{0} - called with FromIndex={1}, TargetInventory={2}, TargetIndex={3}", FString(__FUNCTION__), FromIndex, *GetNameSafe(TargetInventory), TargetIndex);

	if (!TargetInventory)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - TargetInventory is null", FString(__FUNCTION__));
		return;
	}

	if (!OnMoveItemRequested.IsBound())
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - No handler bound for OnMoveItemRequested", FString(__FUNCTION__));
		return;
	}

	OnMoveItemRequested.Broadcast(this, TargetInventory, FromIndex, TargetIndex);
}

void UInventory::MoveItemToOtherInventoryInternal(UInventory* TargetInventory, int32 SourceSlotIndex, int32 TargetSlotIndex)
{
	//UE_LOGFMT(LogProjectA, Log, "{0} - called with SourceSlotIndex={1}, TargetInventory={2}, TargetSlotIndex={3}", FString(__FUNCTION__), SourceSlotIndex, *GetNameSafe(TargetInventory), TargetSlotIndex);

	// Validation
	if (!TargetInventory)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - TargetInventory is null", FString(__FUNCTION__));
		return;
	}

	if (!ReplicatedSlots.Items.IsValidIndex(SourceSlotIndex))
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Invalid SourceSlotIndex {1}", FString(__FUNCTION__), SourceSlotIndex);
		return;
	}

	if (!TargetInventory->ReplicatedSlots.Items.IsValidIndex(TargetSlotIndex))
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Invalid TargetSlotIndex {1}", FString(__FUNCTION__), TargetSlotIndex);
		return;
	}

	const FInventoryReplicatedSlot& SourceRep = ReplicatedSlots.Items[SourceSlotIndex];
	if (SourceRep.IsEmpty())
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Source slot {1} is empty", FString(__FUNCTION__), SourceSlotIndex);
		return;
	}

	if (!TargetInventory->ReplicatedSlots.Items[TargetSlotIndex].IsEmpty())
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Target slot {1} is not empty", FString(__FUNCTION__), TargetSlotIndex);
		return;
	}

	// Move 
	FInventoryReplicatedSlot MovedSlot = SourceRep;
	if (!TargetInventory->ReplicatedSlots.AddToSlot(MovedSlot, TargetSlotIndex))
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Failed to add replicated slot to target inventory", FString(__FUNCTION__));
		return;
	}

	ReplicatedSlots.Items[SourceSlotIndex].Clear();
	ReplicatedSlots.MarkItemDirty(ReplicatedSlots.Items[SourceSlotIndex]);

	MoveLocalRuntimeItemToInventory(TargetInventory, SourceSlotIndex, TargetSlotIndex);

	/*OnInventoryChanged.Broadcast();
	TargetInventory->OnInventoryChanged.Broadcast();*/

	return;
}

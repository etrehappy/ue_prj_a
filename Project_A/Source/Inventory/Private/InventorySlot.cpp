


#include "InventorySlot.h"
#include "Inventory.h"

#include "ProjectALog.h"


///////////////////////////////////////////////////////////////////////////////
/// FInventorySlot
///////////////////////////////////////////////////////////////////////////////

bool FInventorySlot::IsEmpty() const
{
	return Item == nullptr || Item->StackCount <= 0 || Item->Definition == nullptr;
}

void FInventorySlot::Clear()
{
	Item = nullptr;
}


///////////////////////////////////////////////////////////////////////////////
/// FInventoryReplicatedSlot
///////////////////////////////////////////////////////////////////////////////

FInventoryReplicatedSlot::FInventoryReplicatedSlot(const FInventoryReplicatedSlot& Other)
	: FFastArraySerializerItem()
	, DefinitionId(Other.DefinitionId)
	, DefinitionPath(Other.DefinitionPath)
	, CurrentStackCount(Other.CurrentStackCount)
{
}

bool FInventoryReplicatedSlot::IsEmpty() const
{
	return !DefinitionId.IsValid() || CurrentStackCount <= 0;
}

void FInventoryReplicatedSlot::Clear()
{
	DefinitionId = FPrimaryAssetId();
	CurrentStackCount = 0;
}

void FInventoryReplicatedSlot::CopyPayloadFrom(const FInventoryReplicatedSlot& Other)
{
	DefinitionId = Other.DefinitionId;
	DefinitionPath = Other.DefinitionPath;
	CurrentStackCount = Other.CurrentStackCount;	
}


///////////////////////////////////////////////////////////////////////////////
/// FReplicatedSlotArray
///////////////////////////////////////////////////////////////////////////////

bool FReplicatedSlotArray::AddToSlot(const FInventoryReplicatedSlot& NewSlotData, int32 SlotIndex)
{
	UE_LOGFMT(LogProjectA, Log, "{0} - called with SlotIndex={1}", FString(__FUNCTION__), SlotIndex);

	if (NewSlotData.IsEmpty())
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - NewSlot is empty", FString(__FUNCTION__));
		return false;
	}

	if (!Items.IsValidIndex(SlotIndex))
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Invalid SlotIndex {1}", FString(__FUNCTION__), SlotIndex);
		return false;
	}

	if(!Items[SlotIndex].IsEmpty())
	{
		UE_LOGFMT(LogProjectA, Error, "{0} - there is not implementation for non-empty slot at index {1}", FString(__FUNCTION__), SlotIndex);
		return false;
	}

	Items[SlotIndex].DefinitionId = NewSlotData.DefinitionId;
	Items[SlotIndex].DefinitionPath = NewSlotData.DefinitionPath;
	Items[SlotIndex].CurrentStackCount = NewSlotData.CurrentStackCount;
	MarkItemDirty(Items[SlotIndex]);

	return true;
}

bool FReplicatedSlotArray::AddOrStack(const FInventoryReplicatedSlot& NewSlot)
{
	if(NewSlot.IsEmpty())
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - NewSlot is empty", FString(__FUNCTION__));
		return false;
	}

	/*if (Stack(NewSlot))	{ return true; }
	else*/ if (Add(NewSlot)) { return true; }

	return false;
}

void FReplicatedSlotArray::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 Staleness)
{
	UE_LOGFMT(LogProjectA, Log, "{0} - called", FString(__FUNCTION__));

	if (!Owner.IsValid()) 
	{ 
		UE_LOGFMT(LogProjectA, Warning, "{0} - Owner is not valid", FString(__FUNCTION__));
		return; 
	}
	Owner->HandleReplicatedAdd(AddedIndices);
}

void FReplicatedSlotArray::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 Staleness)
{
	UE_LOGFMT(LogProjectA, Log, "{0} - called", FString(__FUNCTION__));

	if (!Owner.IsValid())
	{ 
		UE_LOGFMT(LogProjectA, Warning, "{0} - Owner is not valid", FString(__FUNCTION__));		
		return; 
	}
	Owner->HandleReplicatedChange(ChangedIndices);
}

void FReplicatedSlotArray::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 Staleness)
{
	UE_LOGFMT(LogProjectA, Log, "{0} - called", FString(__FUNCTION__));

	if (!Owner.IsValid())
	{ 
		UE_LOGFMT(LogProjectA, Warning, "{0} - Owner is not valid", FString(__FUNCTION__));
		return; 
	}
	Owner->HandleReplicatedRemove(RemovedIndices);
}

bool FReplicatedSlotArray::NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
{	
	return FastArrayDeltaSerialize<FInventoryReplicatedSlot, FReplicatedSlotArray>(Items, DeltaParms, *this);
}

void FReplicatedSlotArray::SetOwner(UInventory* InOwner)
{
	if(!InOwner)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - InOwner is null", FString(__FUNCTION__));
		return;
	}

	Owner = InOwner;
}

void FReplicatedSlotArray::SetNum(int32 NewNum)
{
	Items.SetNum(NewNum);	
	MarkArrayDirty();
}

bool FReplicatedSlotArray::Stack(const FInventoryReplicatedSlot& NewSlot)
{
	for (FInventoryReplicatedSlot& Slot : Items)
	{	
		if (Slot.DefinitionId == NewSlot.DefinitionId)
		{
			Slot.CurrentStackCount += NewSlot.CurrentStackCount;
			MarkItemDirty(Slot);
			return true;
		}
	}

	return false;
}

bool FReplicatedSlotArray::Add(const FInventoryReplicatedSlot& NewSlot)
{
	for (FInventoryReplicatedSlot& Slot : Items)
	{
		if (Slot.IsEmpty())
		{
			Slot.DefinitionId = NewSlot.DefinitionId;
			Slot.DefinitionPath = NewSlot.DefinitionPath;
			Slot.CurrentStackCount = NewSlot.CurrentStackCount;
			MarkItemDirty(Slot);
			return true;
		}
	}

	return false;
}

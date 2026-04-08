


#include "InventoryComponent.h"
#include "Inventory.h"
#include "Engine/ActorChannel.h"
#include "Net/UnrealNetwork.h"
#include "InventoryItem.h"
#include "StatusEffect/StatusEffectsComponent.h"

#include "ProjectALog.h"

UInventoryComponent::UInventoryComponent()
{	
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	Initialise();
}

void UInventoryComponent::Initialise()
{
	if (!GetOwner())
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Owner is null", FString(__FUNCTION__));
		return;
	}

	// Initialise EquipmentInventory
	FillEquipmentSlots();

	// Initialise Inventory
	if (GetOwner()->HasAuthority() && !Inventory)
	{
		Inventory = NewObject<UInventory>(this);
		Inventory->Initialise(DefaultInventorySize);
		BindInventoryDelegates(Inventory);
	}
}

void UInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UInventoryComponent, Inventory);
	DOREPLIFETIME(UInventoryComponent, EquipmentInventory);
}

//void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
//{
//	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
//
//}

bool UInventoryComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool bWroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	if (Inventory && Channel && Bunch && RepFlags)
	{
		bWroteSomething |= Channel->ReplicateSubobject(Inventory.Get(), *Bunch, *RepFlags);
	}

	if (EquipmentInventory && Channel && Bunch && RepFlags)
	{
		bWroteSomething |= Channel->ReplicateSubobject(EquipmentInventory.Get(), *Bunch, *RepFlags);
	}

	return bWroteSomething;
}

bool UInventoryComponent::AddToInventory(UInventoryItem* Item)
{
	//UE_LOGFMT(LogProjectA, Log, "{0} - called", FString(__FUNCTION__));

	if (!Item)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Item is not UInventoryItem", FString(__FUNCTION__));
		return false;
	}
	//UE_LOGFMT(LogProjectA, Log, "{0} - Item to add: {1}", FString(__FUNCTION__), *Item->GetName());

	if (!this->Inventory) 
	{		
		UE_LOGFMT(LogProjectA, Warning, "{0} - Inventory = nullptr", FString(__FUNCTION__));
		return false;
	}

	//UE_LOGFMT(LogProjectA, Log, "{0} - Adding item {1} to inventory", FString(__FUNCTION__), *Item->GetName());

	bool bIsItemAdded = Inventory->AddItem(Item);

	if (bIsItemAdded) { return true; }

	return false;
}

void UInventoryComponent::AddItemToEquipment(EEquipmentSlot Slot, UInventoryItem* Item)
{
	//UE_LOGFMT(LogProjectA, Log, "{0} - called", FString(__FUNCTION__));
	//UE_LOG(LogProjectA, Log, TEXT("%s - Slot (as int): %d, Item ptr: %p, Item name: %s"), *FString(__FUNCTION__), static_cast<int32>(Slot), Item, (Item ? *Item->GetName() : TEXT("null")));
		
	if (!EquipmentInventory)
	{
		UE_LOG(LogProjectA, Warning, TEXT("%s - EquipmentInventory is null"), *FString(__FUNCTION__));
		return;
	}
		
	const int32 SlotIndex = static_cast<int32>(Slot);
	if (SlotIndex < 0 || SlotIndex >= EquipmentInventory->GetSize())
	{
		UE_LOG(LogProjectA, Warning, TEXT("%s - Invalid SlotIndex %d (Inventory size %d). Aborting."),
			*FString(__FUNCTION__), SlotIndex, EquipmentInventory->GetSize());
		return;
	}
		
	if (!Item)
	{
		UE_LOG(LogProjectA, Warning, TEXT("%s - Item is null"), *FString(__FUNCTION__));
		return;
	}
		
	EquipmentInventory->AddItemToSlot(Item, SlotIndex);	
}

void UInventoryComponent::UseItem(int32 SlotIndex)
{
	UE_LOGFMT(LogProjectA, Log, "{0} - called with SlotIndex: {1}", FString(__FUNCTION__), SlotIndex);

	if (!IsRunningDedicatedServer())
	{
		Server_UseItem(SlotIndex);
		return;
	}

	if (!Inventory)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Inventory is null", FString(__FUNCTION__));
		return;
	}

	UInventoryItem* Item = Inventory->FindItemBySlot(SlotIndex);
	if (!Item || !Item->HasValidData())
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Invalid item in slot {1}", FString(__FUNCTION__), SlotIndex);
		return;
	}

	const UInventoryItemDefinition* Definition = Item->Definition;
	if (!Definition || !Definition->bConsumable || !Definition->EffectTagOnUse.IsValid())
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Item in slot {1} is not consumable", FString(__FUNCTION__), SlotIndex);
		return;
	}

	UStatusEffectComponent* StatusEffectComponent = GetOwner()->FindComponentByClass<UStatusEffectComponent>();
	if (!StatusEffectComponent)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - StatusEffectComponent was not found on owner", FString(__FUNCTION__));
		return;
	}

	StatusEffectComponent->ApplyEffectByTag(Definition->EffectTagOnUse);
	Inventory->UseItemFromSlot(SlotIndex);
}

void UInventoryComponent::UnequipItem(EEquipmentSlot Slot)
{
	UE_LOGFMT(LogProjectA, Error, "{0} - called", FString(__FUNCTION__));
	
	// ...

	// OnEquipmentChanged.Broadcast();
}

UInventoryItem* UInventoryComponent::GetEquippedItem(EEquipmentSlot Slot) const
{
	if (!EquipmentInventory)
	{
		UE_LOGFMT(LogProjectA, Log, "{0} - EquipmentInventory is null", FString(__FUNCTION__));
		return nullptr;
	}

	return EquipmentInventory->FindItemBySlot(static_cast<int32>(Slot));
}

void UInventoryComponent::FillEquipmentSlots()
{
	if (!IsRunningDedicatedServer())
	{
		UE_LOGFMT(LogProjectA, Log, "{0} - Running on client; equipment inventory will be created on server", FString(__FUNCTION__));
		return;
	}

	if (!GetOwner())
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Owner is null", FString(__FUNCTION__));
		return;
	}

	const UEnum* EnumPtr = StaticEnum<EEquipmentSlot>();
	if (!EnumPtr)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Failed to get enum for EEquipmentSlot", FString(__FUNCTION__));
		return;
	}
	
	int32 SlotCount = GetEquipmentSlotNumbers(EnumPtr);	

	// Create or resize the equipment inventory	
	if (!EquipmentInventory)
	{
		EquipmentInventory = NewObject<UInventory>(this);
		EquipmentInventory->Initialise(SlotCount);
		//UE_LOGFMT(LogProjectA, Log, "{0} - Created EquipmentInventory with {1} slots", FString(__FUNCTION__), SlotCount);
	}
	// If inventory exists but size differs, attempt to reinitialize on server.
	else if (EquipmentInventory->GetSize() != SlotCount)
	{
		EquipmentInventory->Initialise(SlotCount);
		//UE_LOGFMT(LogProjectA, Log, "{0} - Reinitialized EquipmentInventory to {1} slots", FString(__FUNCTION__), SlotCount);
	}
	
	BindInventoryDelegates(EquipmentInventory);	
}

int32 UInventoryComponent::GetEquipmentSlotNumbers(const UEnum* EnumPtr) const
{
	// Count enum values (excluding 'None')
	int32 SlotCount = 0;
	for (int32 Index = 0; Index < EnumPtr->NumEnums(); ++Index)
	{
		const int64 Value = EnumPtr->GetValueByIndex(Index);
		const EEquipmentSlot Slot = static_cast<EEquipmentSlot>(Value);

		if (Slot == EEquipmentSlot::None) { continue; }

		++SlotCount;
	}

	//UE_LOGFMT(LogProjectA, Log, "{0} - Equipment slots detected: {1}", FString(__FUNCTION__), SlotCount);
	return SlotCount;
}

void UInventoryComponent::BindInventoryDelegates(UInventory* InInventory)
{
	if (!InInventory) { return;	}

	InInventory->OnMoveItemRequested.RemoveAll(this);
	InInventory->OnMoveItemRequested.AddUniqueDynamic(this, &UInventoryComponent::MoveItemToOtherInventory);

	InInventory->OnRelocateItemRequested.RemoveAll(this);
	InInventory->OnRelocateItemRequested.AddUniqueDynamic(this, &UInventoryComponent::RelocateItemInInventory);
}

void UInventoryComponent::Server_UseItem_Implementation(int32 SlotIndex)
{
	UE_LOGFMT(LogProjectA, Log, "{0} - called with SlotIndex: {1}", FString(__FUNCTION__), SlotIndex);

	UseItem(SlotIndex);
}

void UInventoryComponent::MoveItemToOtherInventory(UInventory* SourceInventory, UInventory* TargetInventory, int32 SourceSlotIndex, int32 TargetSlotIndex)
{
	if (SourceInventory != Inventory && SourceInventory != EquipmentInventory)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - SourceInventory does not belong to this component", FString(__FUNCTION__));
		return;
	}

	if (!SourceInventory || !TargetInventory)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - SourceInventory or TargetInventory is null", FString(__FUNCTION__));
		return;
	}

	if (!GetOwner())
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Owner is null", FString(__FUNCTION__));
		return;
	}

	if (!GetOwner()->HasAuthority())
	{
		Server_MoveItemToOtherInventory(SourceInventory, TargetInventory, SourceSlotIndex, TargetSlotIndex);
		return;
	}

	SourceInventory->MoveItemToOtherInventoryInternal(TargetInventory, SourceSlotIndex, TargetSlotIndex);
}

void UInventoryComponent::Server_MoveItemToOtherInventory_Implementation(UInventory* SourceInventory, UInventory* TargetInventory, int32 SourceSlotIndex, int32 TargetSlotIndex)
{
	MoveItemToOtherInventory(SourceInventory, TargetInventory, SourceSlotIndex, TargetSlotIndex);
}

void UInventoryComponent::RelocateItemInInventory(UInventory* InInventory, int32 FromIndex, int32 ToIndex)
{
	if (InInventory != Inventory && InInventory != EquipmentInventory)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Inventory does not belong to this component", FString(__FUNCTION__));
		return;
	}

	if (!InInventory)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - InInventory is null", FString(__FUNCTION__));
		return;
	}

	if (!GetOwner())
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Owner is null", FString(__FUNCTION__));
		return;
	}

	if (!IsRunningDedicatedServer())
	{
		Server_RelocateItemInInventory(InInventory, FromIndex, ToIndex);
		return;
	}

	InInventory->RelocateItemInInventoryInternal(FromIndex, ToIndex);
}

void UInventoryComponent::Server_RelocateItemInInventory_Implementation(UInventory* InInventory, int32 FromIndex, int32 ToIndex)
{	
	RelocateItemInInventory(InInventory, FromIndex, ToIndex);
}

void UInventoryComponent::OnRep_Inventory()
{
	// Unbind from previous inventory if any
	if (LastBoundInventory && LastBoundInventory != Inventory)
	{
		LastBoundInventory->OnMoveItemRequested.RemoveAll(this);
		LastBoundInventory->OnRelocateItemRequested.RemoveAll(this);
	}

	// Bind to new inventory (safe: AddUniqueDynamic внутри BindInventoryDelegates)
	BindInventoryDelegates(Inventory);
	LastBoundInventory = Inventory;
}

void UInventoryComponent::OnRep_EquipmentInventory()
{
	// Unbind from previous equipment inventory if any
	if (LastBoundEquipmentInventory && LastBoundEquipmentInventory != EquipmentInventory)
	{
		LastBoundEquipmentInventory->OnMoveItemRequested.RemoveAll(this);
		LastBoundEquipmentInventory->OnRelocateItemRequested.RemoveAll(this);
	}

	BindInventoryDelegates(EquipmentInventory);
	LastBoundEquipmentInventory = EquipmentInventory;
}






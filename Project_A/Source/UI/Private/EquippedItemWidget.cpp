#include "EquippedItemWidget.h"

#include "EquippedItemSlotWidget.h"
#include "InventoryComponent.h"
#include "InventorySlot.h"

#include "ProjectALog.h"

void UEquippedItemWidget::InitialiseWithInventory(UInventoryComponent* InInventoryComponent)
{
	if (!InInventoryComponent)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - InInventoryComponent is nullptr", FString(__FUNCTION__));
		return;
	}

	InventoryComponent = InInventoryComponent;
	InventoryComponent->OnEquipmentChanged.AddDynamic(this, &UEquippedItemWidget::HandleEquipmentChanged);

	if (WeaponSlot)
	{
		const int32 WeaponIndex = InventoryComponent->GetEquipmentInventoryIndex(EEquipmentSlot::Weapon);
		WeaponSlot->InitialiseEquipmentSlot(InventoryComponent.Get()->GetEquipmentInventory(), EEquipmentSlot::Weapon, WeaponIndex);
	}
	if (ThrowableSlot)
	{
		const int32 ThrowableIndex = InventoryComponent->GetEquipmentInventoryIndex(EEquipmentSlot::Throwable);
		ThrowableSlot->InitialiseEquipmentSlot(InventoryComponent.Get()->GetEquipmentInventory(), EEquipmentSlot::Throwable, ThrowableIndex);
	}

	HandleEquipmentChanged();
}

void UEquippedItemWidget::HandleEquipmentChanged()
{
	const UInventoryComponent* InventoryComponentPtr = InventoryComponent.Get();
	if (!InventoryComponentPtr)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - InventoryComponent is nullptr", FString(__FUNCTION__));
		return;
	}

	if (WeaponSlot)
	{
		FInventorySlot WeaponSlotData{};
		WeaponSlotData.Item = InventoryComponentPtr->GetEquippedItem(EEquipmentSlot::Weapon);
		WeaponSlot->SetSlotData(WeaponSlotData);
	}

	if (ThrowableSlot)
	{
		FInventorySlot ThrowableSlotData{};
		ThrowableSlotData.Item = InventoryComponentPtr->GetEquippedItem(EEquipmentSlot::Throwable);
		ThrowableSlot->SetSlotData(ThrowableSlotData);
	}
}
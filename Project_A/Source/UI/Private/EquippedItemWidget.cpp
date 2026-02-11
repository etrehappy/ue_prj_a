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

	if (PhysicalWeaponSlot)
	{
		PhysicalWeaponSlot->InitialiseSlot(InventoryComponent.Get()->GetEquipmentInventory(), static_cast<int32>(EEquipmentSlot::Weapon));
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

	if (PhysicalWeaponSlot)
	{
		FInventorySlot WeaponSlot{};
		WeaponSlot.Item = InventoryComponentPtr->GetEquippedItem(EEquipmentSlot::Weapon);
		PhysicalWeaponSlot->SetSlotData(WeaponSlot);
	}
}

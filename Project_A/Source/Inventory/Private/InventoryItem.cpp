


#include "InventoryItem.h"

#include "ProjectALog.h"

bool UInventoryItem::HasValidData() const
{
	bool bHasValidData = true;

	if (!Definition)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Definition is null", FString(__FUNCTION__));
		bHasValidData = false;
		return bHasValidData;
	}

	if (Definition->DisplayName.IsEmpty())
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - DisplayName is empty", FString(__FUNCTION__));
		bHasValidData = false;
	}
	else if(!Definition->Icon)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Icon is invalid", FString(__FUNCTION__));
		bHasValidData = false;
	}
	else if (!Definition->ItemType.IsValid())
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - ItemType is invalid", FString(__FUNCTION__));
		bHasValidData = false;
	}
	else
	{
		bHasValidData = true;
	}
	
	return bHasValidData;	
}

FGameplayTag UInventoryItem::GetItemType() const
{
	if (!HasValidData())
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Item has invalid data, returning an empty tag", FString(__FUNCTION__));
		return FGameplayTag();
	};

	return Definition->ItemType;
}

FPrimaryAssetId UInventoryItemDefinition::GetPrimaryAssetId() const
{
	return FPrimaryAssetId("InventoryItemDefinition", GetFName());
}

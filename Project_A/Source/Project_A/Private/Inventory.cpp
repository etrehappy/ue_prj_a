


#include "Inventory.h"
#include "ItemPickup.h"

#include "ProjectALog.h"


UInventory::UInventory()
{
}

bool UInventory::AddItem(AActor* Item)
{
	if (!Item)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Item is null", FString(__FUNCTION__));
		return false;
	}

	int32 OldNumber = Container.Num();
	Container.Add(Item);

	if (OldNumber < Container.Num())
	{
		UE_LOGFMT(LogProjectA, Log, "{0} - Item {1} added. Total items: {2}", FString(__FUNCTION__), *Item->GetName(), Container.Num());
		return true;
	}	

	return false;
}


//void UInventory::BeginPlay()
//{
//	Super::BeginPlay();	
//}




#include "InventoryComponent.h"
#include "Net/UnrealNetwork.h"

#include "ProjectALog.h"

UInventoryComponent::UInventoryComponent()
{	
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);

	Initialise();
}

void UInventoryComponent::Initialise()
{
	/* Temporary logic */

	Inventory = CreateDefaultSubobject<UInventory>(TEXT("Inventory"));
}

bool UInventoryComponent::AddToInventory(AActor* Item)
{
	bool bIsItemAdded = Inventory->AddItem(Item);

	if (bIsItemAdded) { return true; }

	return false;
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}




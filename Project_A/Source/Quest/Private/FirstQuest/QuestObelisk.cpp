#include "FirstQuest/QuestObelisk.h"
#include "InventoryComponent.h"
#include "QuestLogComponent.h"
#include "Net/UnrealNetwork.h"

#include "ProjectALog.h"

void AQuestObelisk::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AQuestObelisk, bIsActivated);
}

bool AQuestObelisk::CanInteract(APawn* Interactor) const
{
	return Interactor && !bIsActivated;
}

void AQuestObelisk::Interact(APawn* Interactor)
{
	if (!Interactor || !Interactor->HasAuthority() || bIsActivated)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Interactor is invalid, does not have authority, or obelisk is already activated", FString(__FUNCTION__));
		return;
	}

	UQuestLogComponent* QuestLogComponent = Interactor->FindComponentByClass<UQuestLogComponent>();
	if (!QuestLogComponent || !QuestLogComponent->IsInsertStoneQuestAccepted())
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Interactor does not have a QuestLogComponent or has not accepted the insert stone quest", FString(__FUNCTION__));
		return;
	}

	UInventoryComponent* InventoryComponent = Interactor->FindComponentByClass<UInventoryComponent>();
	if (!InventoryComponent)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Interactor does not have an InventoryComponent", FString(__FUNCTION__));
		return;
	}

	if (!InventoryComponent->ConsumeItemsByTag(RequiredStoneItemTag, RequiredStoneCount))
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Interactor does not have enough items with tag {1}", FString(__FUNCTION__), *RequiredStoneItemTag.ToString());
		return;
	}

	bIsActivated = true;
	QuestLogComponent->CompleteInsertStoneQuest();
	OnObeliskActivated();
}

void AQuestObelisk::OnRep_IsActivated()
{
	if (bIsActivated)
	{
		OnObeliskActivated();
	}
}
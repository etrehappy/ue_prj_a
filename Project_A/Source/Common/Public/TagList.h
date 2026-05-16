#pragma once

#include "CoreMinimal.h"

#include "GameplayTagContainer.h"



namespace TagListInteraction
{
	const FGameplayTag ItemPickup = FGameplayTag::RequestGameplayTag(FName(TEXT("Interaction.Item.Pickup")));
	const FGameplayTag QuestAccept = FGameplayTag::RequestGameplayTag(FName(TEXT("Interaction.Quest.Accept")));
	const FGameplayTag PartyInvite = FGameplayTag::RequestGameplayTag(FName(TEXT("Interaction.Party.Invite")));
	const FGameplayTag InventoryOpen = FGameplayTag::RequestGameplayTag(FName(TEXT("Interaction.Inventory.Open")));
}
#include "QuestLogComponent.h"
#include "Net/UnrealNetwork.h"

#include "ProjectALog.h"

UQuestLogComponent::UQuestLogComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UQuestLogComponent::BeginPlay()
{
	Super::BeginPlay();
	BroadcastQuestState();
}

void UQuestLogComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UQuestLogComponent, CurrentQuestState);
}

void UQuestLogComponent::AcceptInsertStoneQuest()
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Owner is invalid or does not have authority", FString(__FUNCTION__));
		return;
	}

	if (CurrentQuestState != EQuestState::None)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Quest is already accepted or completed", FString(__FUNCTION__));
		return;
	}

	CurrentQuestState = EQuestState::Accepted_InsertStone;
	BroadcastQuestState();
}

void UQuestLogComponent::CompleteInsertStoneQuest()
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Owner is invalid or does not have authority", FString(__FUNCTION__));
		return;
	}

	if (CurrentQuestState != EQuestState::Accepted_InsertStone)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Quest is not in the correct state to be completed", FString(__FUNCTION__));
		return;
	}

	CurrentQuestState = EQuestState::Completed_InsertStone;
	BroadcastQuestState();
}

// A temporary solution
FText UQuestLogComponent::GetCurrentQuestText() const
{
	switch (CurrentQuestState)
	{
	case EQuestState::Accepted_InsertStone:
		return FText::FromString(TEXT("Quest: Insert the stone into the obelisk"));
	case EQuestState::Completed_InsertStone:
		return FText::FromString(TEXT("The quest is completed: the stone is inserted into the obelisk."));
	default:
		return FText::FromString(TEXT("There is no active quest"));
	}
}

void UQuestLogComponent::OnRep_CurrentQuestState()
{
	BroadcastQuestState();
}

void UQuestLogComponent::BroadcastQuestState() const
{
	OnQuestStateChanged.Broadcast(CurrentQuestState, GetCurrentQuestText());
}
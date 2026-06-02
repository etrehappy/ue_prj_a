

#include "PlayerDialogueComponent.h"

#include "Engine/World.h"
#include "GameFramework/Pawn.h"

#include "ProjectALog.h"

UPlayerDialogueComponent::UPlayerDialogueComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

APawn* UPlayerDialogueComponent::GetOwnerPawn() const
{
	return Cast<APawn>(GetOwner());
}

UDialogueManagerSubsystem* UPlayerDialogueComponent::GetDialogueSubsystem(APawn* PlayerPawn) const
{	
	if (!PlayerPawn)
	{
		UE_LOGFMT(LogProjectA, Log, "{0} - Owner is not a Pawn", FString(__FUNCTION__));
		return nullptr;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOGFMT(LogProjectA, Log, "{0} - World is nullptr", FString(__FUNCTION__));
		return nullptr;
	}

	UDialogueManagerSubsystem* DialogueSubsystem = World->GetSubsystem<UDialogueManagerSubsystem>();
	if (!DialogueSubsystem)
	{
		UE_LOGFMT(LogProjectA, Log, "{0} - DialogueSubsystem is nullptr", FString(__FUNCTION__));
		return nullptr;
	}

	return DialogueSubsystem;
}

void UPlayerDialogueComponent::RequestDialogueSnapshot()
{
	Server_RequestDialogueSnapshot();
}

void UPlayerDialogueComponent::SubmitDialogueChoice(FGuid SessionId, FName ChoiceId)
{
	Server_SubmitDialogueChoice(SessionId, ChoiceId);
}

void UPlayerDialogueComponent::CloseDialogue()
{
	Server_CloseDialogue();
}

void UPlayerDialogueComponent::Server_RequestDialogueSnapshot_Implementation()
{
	APawn* PlayerPawn = GetOwnerPawn();
	auto DialogueSubsystem = GetDialogueSubsystem(PlayerPawn);
	if (!DialogueSubsystem)	{
		
		UE_LOGFMT(LogProjectA, Warning, "{0} - DialogueSubsystem is nullptr", FString(__FUNCTION__));
		Client_DialogueClosed();
		return;
	}

	FDialogueNodeRuntime Node{};
	if (DialogueSubsystem->GetActiveDialogueNode(PlayerPawn, Node))
	{
		Client_DialogueNodeAvailable(Node);
	}
	else
	{
		Client_DialogueClosed();
		UE_LOGFMT(LogProjectA, Log, "{0} - No active dialogue for player", FString(__FUNCTION__));
	}
}

void UPlayerDialogueComponent::Server_CloseDialogue_Implementation()
{
	APawn* PlayerPawn = GetOwnerPawn();
	auto DialogueSubsystem = GetDialogueSubsystem(PlayerPawn);
	if (!DialogueSubsystem) 
	{ 
		UE_LOGFMT(LogProjectA, Warning, "{0} - DialogueSubsystem is nullptr", FString(__FUNCTION__));
		return; 
	}

	DialogueSubsystem->TryCloseDialogue(PlayerPawn);
	Client_DialogueClosed();
}

void UPlayerDialogueComponent::Client_DialogueNodeAvailable_Implementation(const FDialogueNodeRuntime& Node)
{
	OnDialogueNodeReceived.Broadcast(Node);
}

void UPlayerDialogueComponent::Client_DialogueClosed_Implementation()
{
	OnDialogueClosed.Broadcast();
}

void UPlayerDialogueComponent::Server_SubmitDialogueChoice_Implementation(FGuid SessionId, FName ChoiceId)
{
	if (!SessionId.IsValid() || ChoiceId.IsNone())
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Invalid SessionId or ChoiceId", FString(__FUNCTION__));
		return;
	}

	APawn* PlayerPawn = GetOwnerPawn();
	auto DialogueSubsystem = GetDialogueSubsystem(PlayerPawn);
	if (!DialogueSubsystem)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - DialogueSubsystem is nullptr", FString(__FUNCTION__));
		return;
	}


	FDialogueNodeRuntime Node{};
	bool bChoiceSelected = DialogueSubsystem->TrySelectChoice(PlayerPawn, SessionId, ChoiceId, Node);
	if (!bChoiceSelected)
	{
		UE_LOGFMT(LogProjectA, Log, "{0} - Failed to select choice. SessionId: {1}, ChoiceId: {2}", FString(__FUNCTION__), SessionId, ChoiceId);
		return;
	}

	if (Node.bIsDialogueEnded)
	{
		Client_DialogueClosed();
	}
	else
	{
		Client_DialogueNodeAvailable(Node);
	}	
}


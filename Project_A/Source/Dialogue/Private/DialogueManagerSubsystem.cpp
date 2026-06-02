#include "DialogueManagerSubsystem.h"

#include "DialogueDefinition.h"
#include "CustomDialogueTypes.h"

#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerState.h"
#include "Containers/Array.h"

#include "ProjectALog.h"

namespace
{
	constexpr double InitialLastChoiceRequestTime = -1.0;
}


void UDialogueManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	DialogueDefinitionsById.Reset();
	ActiveSessionsByPlayer.Reset();

	LoadDialogueDefinitionsFromConfig();
}

void UDialogueManagerSubsystem::Deinitialize()
{
	ActiveSessionsByPlayer.Reset();
	DialogueDefinitionsById.Reset();

	Super::Deinitialize();
}

void UDialogueManagerSubsystem::LoadDialogueDefinitionsFromConfig()
{
	for (const TSoftObjectPtr<UDialogueDefinition>& DialogueDefinitionSoft : StartupDialogueDefinitions)
	{
		if (DialogueDefinitionSoft.IsNull())
		{
			continue;
		}

		UDialogueDefinition* DialogueDefinition = DialogueDefinitionSoft.LoadSynchronous();
		if (!DialogueDefinition)
		{
			UE_LOGFMT(LogProjectA, Warning, "{0} - Failed to load dialogue definition", FString(__FUNCTION__));
			continue;
		}

		RegisterDialogueDefinition(DialogueDefinition);
	}
}



///////////////////////////////////////////////////////////////////////////////
/// 
/// API
/// 
///////////////////////////////////////////////////////////////////////////////

const UDialogueDefinition* UDialogueManagerSubsystem::GetDialogueDefinition(FName DialogueId) const
{
	if (DialogueId.IsNone())
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Invalid DialogueId: None", FString(__FUNCTION__));
		return nullptr;
	}

	const TObjectPtr<UDialogueDefinition>* Dialogue = DialogueDefinitionsById.Find(DialogueId);

	if (!Dialogue)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Dialogue definition not found: {1}", FString(__FUNCTION__), DialogueId.ToString());
		return nullptr;
	}

	return Dialogue->Get();
}

bool UDialogueManagerSubsystem::TryStartDialogue(APawn* PlayerPawn, AActor* NpcActor, FName DialogueId, FDialogueNodeRuntime& OutNode)
{
	OutNode = FDialogueNodeRuntime{};

	// 1. Validation

	if (!PlayerPawn || !IsRunningDedicatedServer() || !NpcActor || DialogueId.IsNone())
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Invalid params or no authority", FString(__FUNCTION__));
		return false;
	}

	// cleanup stale sessions and enforce max active sessions
	CleanupInvalidSessions();

	if (ActiveSessionsByPlayer.Num() >= MaxActiveSessions)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - MaxActiveSessions reached", FString(__FUNCTION__));
		return false;
	}

	const UDialogueDefinition* Definition = GetDialogueDefinition(DialogueId);
	if (!Definition)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Dialogue definition not found: {1}", FString(__FUNCTION__), DialogueId.ToString());
		return false;
	}

	// 2. Preparation

	FDialogueSession Session{};
	Session.SessionId = FGuid::NewGuid();
	Session.DialogueId = DialogueId;
	Session.PlayerPawn = PlayerPawn;
	Session.NpcActor = NpcActor;
	Session.Definition = Definition;
	Session.Revision = 0;

    // Set initial runtime metadata
	const FName PlayerKey = MakePlayerKey(PlayerPawn);
	const double Now = GetServerTimeSeconds();
	Session.LastActivityServerTime = Now;
	Session.LastChoiceRequestServerTime = InitialLastChoiceRequestTime;


	// 3. Start dialogue node
	if (!EnterNode(Session, Definition->StartNodeId, NAME_None, OutNode))
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Failed to enter start node: {1}", FString(__FUNCTION__), Definition->StartNodeId.ToString());
		return false;
	}

	if (OutNode.bIsDialogueEnded)
	{
		// finished immediately, do not add to active sessions but keep persistent state
		return true;
	}

	// 4. Save session
	ActiveSessionsByPlayer.Add(PlayerKey, Session);
	return true;
}

bool UDialogueManagerSubsystem::TrySelectChoice(APawn* PlayerPawn, FGuid SessionId, FName ChoiceId, FDialogueNodeRuntime& OutNode)
{
	OutNode = FDialogueNodeRuntime{};

	// validation
	if (!PlayerPawn || !IsRunningDedicatedServer() || !SessionId.IsValid() || ChoiceId.IsNone())
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Invalid params or no authority", FString(__FUNCTION__));
		return false;
	}

	// Find active session
	FDialogueSession* Session = FindSession(PlayerPawn);
	if (!Session || !Session->Definition.IsValid())
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Active session not found", FString(__FUNCTION__));
		return false;
	}

	// Ensure client references the correct session
	if (Session->SessionId != SessionId)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - SessionId mismatch", FString(__FUNCTION__));
		return false;
	}

	// Rate limit per-session (updates session timestamps on success)
	if (!TouchAndValidateChoiceRate(*Session))
	{
		UE_LOGFMT(LogProjectA, Log, "{0} - Choice request rate-limited", FString(__FUNCTION__));
		return false;
	}

	// Get the selected choice from the current node
	const FDialogueChoiceDefinition* SelectedChoice = GetSelectedChoice(*Session, ChoiceId);
	if (!SelectedChoice)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Choice not found: {1}", FString(__FUNCTION__), ChoiceId.ToString());
		return false;
	}

	// Check choice availability conditions
	if (!AreConditionsSatisfied(SelectedChoice->Conditions, *Session, ChoiceId, SelectedChoice->ConditionsMatchMode))
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Choice conditions failed: {1}", FString(__FUNCTION__), ChoiceId.ToString());
		return false;
	}

	// Apply immediate effects of the choice
	ApplyEffects(SelectedChoice->Effects, *Session, ChoiceId);

	const FName PlayerKey = MakePlayerKey(PlayerPawn);

	// If this choice closes the dialogue, mark end and cleanup session
	if (SelectedChoice->bCloseDialogue)
	{
		OutNode.SessionId = Session->SessionId;
		OutNode.DialogueId = Session->DialogueId;
		OutNode.NodeId = Session->CurrentNodeId;
		OutNode.bIsDialogueEnded = true;
		OutNode.Revision = Session->Revision;

		ActiveSessionsByPlayer.Remove(PlayerKey);
		return true;
	}

	// Otherwise, move session to target node
	if (!EnterNode(*Session, SelectedChoice->TargetNodeId, ChoiceId, OutNode))
	{
		return false;
	}

	// If the new node ends the dialogue, remove session
	if (OutNode.bIsDialogueEnded)
	{
		ActiveSessionsByPlayer.Remove(PlayerKey);
	}

	return true;
}

bool UDialogueManagerSubsystem::TryCloseDialogue(APawn* PlayerPawn)
{
	if (!PlayerPawn || !PlayerPawn->HasAuthority())
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Invalid player or no authority", FString(__FUNCTION__));
		return false;
	}

	const FName Key = MakePlayerKey(PlayerPawn);
   return ActiveSessionsByPlayer.Remove(Key) > 0;
}

bool UDialogueManagerSubsystem::GetActiveDialogueNode(const APawn* PlayerPawn, FDialogueNodeRuntime& OutNode)
{
	OutNode = FDialogueNodeRuntime{};

	FDialogueSession* Session = FindSession(PlayerPawn);
	if (!Session || !Session->Definition.IsValid())
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Active session not found", FString(__FUNCTION__));
		return false;
	}

	const UDialogueDefinition* Definition = Session->Definition.Get();
	const FDialogueNodeDefinition* Node = Definition ? Definition->FindNodeById(Session->CurrentNodeId) : nullptr;
	if (!Node)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Current node not found: {1}", FString(__FUNCTION__), Session->CurrentNodeId.ToString());
		return false;
	}

	BuildNodeRuntime(*Session, *Node, OutNode);
	return true;
}




///////////////////////////////////////////////////////////////////////////////
/// 
/// Helpers
/// 
///////////////////////////////////////////////////////////////////////////////

void UDialogueManagerSubsystem::RegisterDialogueDefinition(UDialogueDefinition* DialogueDefinition)
{
	if (!DialogueDefinition)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - DialogueDefinition is null", FString(__FUNCTION__));
		return;
	}

	if (!DialogueDefinition->IsDefinitionValid())
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Invalid dialogue definition '{1}'", FString(__FUNCTION__), DialogueDefinition->GetName());
		return;
	}

	if (DialogueDefinitionsById.Contains(DialogueDefinition->DialogueId))
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Duplicate DialogueId '{1}', overriding", FString(__FUNCTION__),
			DialogueDefinition->DialogueId.ToString());
	}

	DialogueDefinitionsById.Add(DialogueDefinition->DialogueId, DialogueDefinition);
}

double UDialogueManagerSubsystem::GetServerTimeSeconds() const
{
	const UWorld* World = GetWorld();
	return World ? World->GetTimeSeconds() : 0.0;
}

const FDialogueChoiceDefinition* UDialogueManagerSubsystem::GetSelectedChoice(const FDialogueSession& Session, FName ChoiceId) const
{
	const UDialogueDefinition* Definition = Session.Definition.Get();
	const FDialogueNodeDefinition* CurrentNode = Definition ? Definition->FindNodeById(Session.CurrentNodeId) : nullptr;
	if (!CurrentNode)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Current node not found", FString(__FUNCTION__));
		return nullptr;
	}

	const FDialogueChoiceDefinition* SelectedChoice = nullptr;
	for (const TObjectPtr<UDialogueChoiceDataAsset>& ChoiceAsset : CurrentNode->Choices)
	{
		if (!ChoiceAsset)
		{
			continue;
		}

		const FDialogueChoiceDefinition& Choice = ChoiceAsset->Choice;
		if (Choice.ChoiceId == ChoiceId)
		{
			SelectedChoice = &Choice;
			break;
		}
	}

	return SelectedChoice;
}

FName UDialogueManagerSubsystem::MakePlayerKey(const APawn* Pawn)
{
	if (!Pawn)
	{
		return NAME_None;
	}

	if (APlayerState* PlayerState = Pawn->GetPlayerState())
	{
		// Use player state name as key 
		return FName(*PlayerState->GetPlayerName());
	}

	// Fall back to pawn's FName
	return Pawn->GetFName();
}

void UDialogueManagerSubsystem::CleanupInvalidSessions()
{
	const double Now = GetServerTimeSeconds();
	TArray<FName> KeysToRemove;

	for (const TPair<FName, FDialogueSession>& Pair : ActiveSessionsByPlayer)
	{
		const FDialogueSession& Session = Pair.Value;
		const bool bInvalidRefs = !Session.PlayerPawn.IsValid() 
			|| !Session.NpcActor.IsValid() 
			|| !Session.Definition.IsValid();
		const double LastActivity = Session.LastActivityServerTime;
		const bool bTimedOut = (Now - LastActivity) > static_cast<double>(SessionIdleTimeoutSeconds);

		if (bInvalidRefs || bTimedOut)
		{
			KeysToRemove.Add(Pair.Key);
		}
	}

	for (const FName& Key : KeysToRemove)
	{
		ActiveSessionsByPlayer.Remove(Key);
	}
}

bool UDialogueManagerSubsystem::EnterNode(FDialogueSession& Session, FName NextNodeId, FName SelectedChoiceId, FDialogueNodeRuntime& OutNode)
{
	const UDialogueDefinition* Definition = Session.Definition.Get();
	if (!Definition || NextNodeId.IsNone())
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Invalid definition or NextNodeId", FString(__FUNCTION__));
		return false;
	}

	const FDialogueNodeDefinition* Node = Definition->FindNodeById(NextNodeId);
	if (!Node)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Node not found: {1}", FString(__FUNCTION__), NextNodeId.ToString());
		return false;
	}

	if (!AreConditionsSatisfied(Node->EnterConditions, Session, SelectedChoiceId, Node->EnterConditionsMatchMode))
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Node enter conditions failed: {1}", FString(__FUNCTION__), NextNodeId.ToString());
		return false;
	}

	ApplyEffects(Node->EnterEffects, Session, SelectedChoiceId);

	Session.CurrentNodeId = NextNodeId;
	Session.LastActivityServerTime = GetServerTimeSeconds();
	++Session.Revision;
		
	BuildNodeRuntime(Session, *Node, OutNode);

	return true;
}

void UDialogueManagerSubsystem::BuildNodeRuntime(const FDialogueSession& Session, const FDialogueNodeDefinition& Node, FDialogueNodeRuntime& OutNode) const
{
	OutNode = FDialogueNodeRuntime{};
	OutNode.SessionId = Session.SessionId;
	OutNode.DialogueId = Session.DialogueId;
	OutNode.NodeId = Node.NodeId;
	OutNode.SpeakerId = Node.SpeakerId;
	OutNode.Text = Node.Text;
	OutNode.Revision = Session.Revision;

	OutNode.Choices.Reserve(Node.Choices.Num());
	for (const TObjectPtr<UDialogueChoiceDataAsset>& ChoiceAsset : Node.Choices)
	{
		if (!ChoiceAsset)
		{
			continue;
		}

		const FDialogueChoiceDefinition& Choice = ChoiceAsset->Choice;

		if (!AreConditionsSatisfied(Choice.Conditions, Session, Choice.ChoiceId, Choice.ConditionsMatchMode))
		{
			continue;
		}

		FDialogueChoiceRuntime RuntimeChoice{};
		RuntimeChoice.ChoiceId = Choice.ChoiceId;
		RuntimeChoice.Text = Choice.Text;
		OutNode.Choices.Add(RuntimeChoice);
	}
}

bool UDialogueManagerSubsystem::AreConditionsSatisfied(const TArray<TObjectPtr<UDialogueCondition>>& Conditions,
	const FDialogueSession& Session, FName SelectedChoiceId, EDialogueConditionMatchMode MatchMode) const
{
	if (Conditions.Num() == 0)
	{
		//UE_LOGFMT(LogProjectA, Log, "{0} - No conditions to check, automatically satisfied", FString(__FUNCTION__));
		return true;
	}

	const FDialogueRuntimeContext RuntimeContext = BuildRuntimeContext(Session, SelectedChoiceId);

	if (MatchMode == EDialogueConditionMatchMode::Any)
	{
		for (const UDialogueCondition* Condition : Conditions)
		{
			if (Condition && Condition->IsSatisfied(RuntimeContext))
			{
				return true;
			}
		}
		return false;
	}

	// All (AND)
	for (const UDialogueCondition* Condition : Conditions)
	{
		if (Condition && !Condition->IsSatisfied(RuntimeContext))
		{
			return false;
		}
	}

	return true;
}

void UDialogueManagerSubsystem::ApplyEffects(const TArray<TObjectPtr<UDialogueEffect>>& Effects, const FDialogueSession& Session, FName SelectedChoiceId) const
{
	const FDialogueRuntimeContext RuntimeContext = BuildRuntimeContext(Session, SelectedChoiceId);

	for (const UDialogueEffect* Effect : Effects)
	{
		if (!Effect)
		{
			continue;
		}

		Effect->Apply(RuntimeContext);
	}
}

FDialogueRuntimeContext UDialogueManagerSubsystem::BuildRuntimeContext(const FDialogueSession& Session, FName SelectedChoiceId) const
{
	FDialogueRuntimeContext RuntimeContext{};
	RuntimeContext.World = GetWorld();
	RuntimeContext.PlayerPawn = Session.PlayerPawn.Get();
	RuntimeContext.NpcActor = Session.NpcActor.Get();
	RuntimeContext.DialogueDefinition = Session.Definition.Get();
	RuntimeContext.CurrentNodeId = Session.CurrentNodeId;
	RuntimeContext.SelectedChoiceId = SelectedChoiceId;
	return RuntimeContext;
}

bool UDialogueManagerSubsystem::TouchAndValidateChoiceRate(FDialogueSession& Session)
{
	const double Now = GetServerTimeSeconds();
	const double Delta = Now - Session.LastChoiceRequestServerTime;

	if (Delta < static_cast<double>(MinChoiceRequestIntervalSeconds))
	{		
		return false;
	}

	Session.LastChoiceRequestServerTime = Now;
	Session.LastActivityServerTime = Now;
	return true;
}

FDialogueSession* UDialogueManagerSubsystem::FindSession(const APawn* PlayerPawn)
{
	if (!PlayerPawn)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - PlayerPawn is null", FString(__FUNCTION__));
		return nullptr;
	}

	return ActiveSessionsByPlayer.Find(MakePlayerKey(PlayerPawn));
}


#include "DialogueDefinition.h"

#include "Containers/Set.h"

#include "ProjectALog.h"

namespace
{
	constexpr int32 MaxNodes = 256;
	constexpr int32 MaxChoicesPerNode = 8;
}

////////////////////////
/// 
/// UDialogueDefinition
/// 
////////////////////////

const FDialogueNodeDefinition* UDialogueDefinition::FindNodeById(FName NodeId) const
{
	if (NodeId.IsNone())
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - NodeId is None", FString(__FUNCTION__));
		return nullptr;
	}

	for (const FDialogueNodeDefinition& Node : Nodes)
	{
		if (Node.NodeId == NodeId)
		{
			return &Node;
		}
	}

	UE_LOGFMT(LogProjectA, Log, "{0} - NodeId '{1}' not found in dialogue '{2}'", FString(__FUNCTION__), *NodeId.ToString(), *DialogueId.ToString());

	return nullptr;
}

bool UDialogueDefinition::IsDefinitionValid() const
{
	// 1
	if (!IsDefinitionСomplete())
	{
		return false;
	}

	// 2
	if (Nodes.Num() > ::MaxNodes)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Too many nodes: {1}. Maximum allowed is {2}.", FString(__FUNCTION__), Nodes.Num(), ::MaxNodes);
		return false;
	}

	// 3
	TSet<FName> NodeIds{};
	for (const FDialogueNodeDefinition& Node : Nodes)
	{
		if (!IsNodeValid(Node, NodeIds, DialogueId))
		{
			return false;
		}
	}

	// 4
	if (!NodeIds.Contains(StartNodeId))
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - StartNodeId '{1}' does not exist in Nodes.", FString(__FUNCTION__), *StartNodeId.ToString());
		return false;
	}

	// 5
	if (!AreChoiceTargetsValid(NodeIds))
	{
		return false;
	}	

	return true;
}

bool UDialogueDefinition::IsDefinitionСomplete() const
{
	if (DialogueId.IsNone())
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - DialogueId is None", FString(__FUNCTION__));
		return false;
	}

	if (StartNodeId.IsNone())
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - StartNodeId is None", FString(__FUNCTION__));
		return false;
	}

	if (Nodes.Num() == 0)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Nodes array is empty.", FString(__FUNCTION__));
		return false;
	}

	return true;
}

bool UDialogueDefinition::IsChoiceValid(const FDialogueNodeDefinition& Node) const
{
	TSet<FName> ChoiceIds{};
	for (const TObjectPtr<UDialogueChoiceDataAsset>& ChoiceAsset : Node.Choices)
	{
		if (!ChoiceAsset)
		{
			UE_LOGFMT(LogProjectA, Warning, "{0} - Node '{1}' contains null Choice asset.", FString(__FUNCTION__), *Node.NodeId.ToString());
			return false;
		}

		const FDialogueChoiceDefinition& Choice = ChoiceAsset->Choice;

		if (Choice.ChoiceId.IsNone())
		{
			UE_LOGFMT(LogProjectA, Warning, "{0} - Choice with None ChoiceId found in node '{1}'.", FString(__FUNCTION__), *Node.NodeId.ToString());
			return false;
		}

		if (ChoiceIds.Contains(Choice.ChoiceId))
		{
			UE_LOGFMT(LogProjectA, Warning, "{0} - Duplicate ChoiceId '{1}' found in node '{2}' of dialogue '{3}'.", FString(__FUNCTION__), *Choice.ChoiceId.ToString(), *Node.NodeId.ToString(), *DialogueId.ToString());
			return false;
		}

		ChoiceIds.Add(Choice.ChoiceId);

		if (!Choice.bCloseDialogue && Choice.TargetNodeId.IsNone())
		{
			UE_LOGFMT(LogProjectA, Warning, "{0} - Choice '{1}' in node '{2}' must have a TargetNodeId if it does not close the dialogue.", FString(__FUNCTION__), *Choice.ChoiceId.ToString(), *Node.NodeId.ToString());
			return false;
		}
	}

	return true;
}

bool UDialogueDefinition::AreChoiceTargetsValid(const TSet<FName>& NodeIds) const
{
	for (const FDialogueNodeDefinition& Node : Nodes)
	{
		for (const TObjectPtr<UDialogueChoiceDataAsset>& ChoiceAsset : Node.Choices)
		{
			if (!ChoiceAsset)
			{
				UE_LOGFMT(LogProjectA, Warning, "{0} - Node '{1}' contains null Choice asset.", FString(__FUNCTION__), *Node.NodeId.ToString());
				return false;
			}

			const FDialogueChoiceDefinition& Choice = ChoiceAsset->Choice;

			if (!Choice.bCloseDialogue && !Choice.TargetNodeId.IsNone() && !NodeIds.Contains(Choice.TargetNodeId))
			{
				UE_LOGFMT(LogProjectA, Warning, "{0} - Choice '{1}' in node '{2}' has invalid TargetNodeId '{3}' that does not exist in dialogue '{4}'.", FString(__FUNCTION__), *Choice.ChoiceId.ToString(), *Node.NodeId.ToString(), *Choice.TargetNodeId.ToString(), *DialogueId.ToString());
				return false;
			}
		}
	}

	return true;
}

bool UDialogueDefinition::IsNodeValid(const FDialogueNodeDefinition& Node, TSet<FName>& NodeIds, const FName& InDialogueId) const
{
	// 1
	if (Node.NodeId.IsNone())
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - NodeId is None for a node with text: '{1}'", FString(__FUNCTION__), Node.Text.ToString());
		return false;
	}

	// 2
	if (NodeIds.Contains(Node.NodeId))
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Duplicate NodeId '{1}' found in dialogue '{2}'.", FString(__FUNCTION__), *Node.NodeId.ToString(), *InDialogueId.ToString());
		return false;
	}


	NodeIds.Add(Node.NodeId);

	// 3
	if (Node.Choices.Num() > MaxChoicesPerNode)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Too many choices in node '{1}': {2}. Maximum allowed is {3}.", FString(__FUNCTION__), *Node.NodeId.ToString(), Node.Choices.Num(), ::MaxChoicesPerNode);
		return false;
	}

	// 4
	if (!IsChoiceValid(Node))
	{
		return false;
	}

	return true;
}


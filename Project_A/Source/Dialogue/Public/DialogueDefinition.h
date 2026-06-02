/*****************************************************************//**
 * \file   DialogueDefinition.h
 * \brief  Contains:
 * 
 * - struct FDialogueChoiceDefinition — player's choice in dialogue node
 * - struct FDialogueNodeDefinition — NPC phrase + optional player choices
 * - class	UDialogueDefinition — dialogue definition containing nodes and choices
 * 
 * - enum class EDialogueConditionMatchMode
 * - struct FDialogueRuntimeContext — contains runtime information
 * 
 * - class	UDialogueChoiceDataAsset
 * - class	UDialogueNodeDataAsset
 * 
 * \date   May 2026
 *********************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CustomDialogueTypes.h"

#include "DialogueDefinition.generated.h"

class AActor;
class APawn;
class UWorld;

/**
 * @enum EDialogueConditionMatchMode
 * @brief Specifies how dialogue conditions are evaluated.
 * @see FDialogueNodeDefinition
 * @see FDialogueChoiceDefinition
 */
UENUM(BlueprintType)
enum class EDialogueConditionMatchMode : uint8
{
	All UMETA(DisplayName = "All (AND)"),
	Any UMETA(DisplayName = "Any (OR)")
};

/**
 * @struct FDialogueChoiceDefinition
 * @brief A player choice in a dialogue node.
 * @see FDialogueNodeDefinition
 */
USTRUCT(BlueprintType)
struct FDialogueChoiceDefinition
{
	GENERATED_BODY()

	/**
	 * @brief Unique identifier of the choice inside dialogue definition.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dialogue|Choice")
	FName ChoiceId{NAME_None};

	/**
	 * @brief Text shown to player.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dialogue|Choice")
	FText Text{};

	/**
	 * @brief Target node to jump to after selecting this choice.
	 * Can be empty only if bCloseDialogue is true.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dialogue|Choice")
	FName TargetNodeId{NAME_None};

	/**
	 * @brief If true, dialogue session should be closed after this choice.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dialogue|Choice")
	bool bCloseDialogue{false};

	/**
	 * @brief Specifies how conditions are evaluated for this choice.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dialogue|Choice")
	EDialogueConditionMatchMode ConditionsMatchMode{EDialogueConditionMatchMode::All};

	/**
	 * @brief Static conditions required for choice availability. For example, to show/hide dialogue options based on quest state.
	 */
	UPROPERTY(EditDefaultsOnly, Instanced, BlueprintReadOnly, Category = "Dialogue|Choice")
	TArray<TObjectPtr<UDialogueCondition>> Conditions{};

	/**
	 * @brief Static effects executed after choice is selected. For example, to update quest state.
	 */
	UPROPERTY(EditDefaultsOnly, Instanced, BlueprintReadOnly, Category = "Dialogue|Choice")
	TArray<TObjectPtr<UDialogueEffect>> Effects{};
};

/**
 * @struct FDialogueNodeDefinition
 * @brief A dialogue node (NPC phrase + optional player choices).
 */
USTRUCT(BlueprintType)
struct FDialogueNodeDefinition
{
	GENERATED_BODY()

	/**
	 * @brief Unique node identifier in dialogue definition.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dialogue|Node")
	FName NodeId{NAME_None};

	/**
	 * @brief Optional speaker identifier (for UI portrait/name mapping).
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dialogue|Node")
	FName SpeakerId{NAME_None};

	/**
	 * @brief Node kind.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dialogue|Node")
	EDialogueNodeKind NodeKind{EDialogueNodeKind::Line};

	/**
	 * @brief NPC line text.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dialogue|Node", meta = (MultiLine = "true"))
	FText Text{};

	/**
	 * @brief Specifies how conditions are evaluated for entering this node.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dialogue|Node")
	EDialogueConditionMatchMode EnterConditionsMatchMode{EDialogueConditionMatchMode::All};

	/**
	 * @brief Static conditions required to enter this node. For example, to show/hide dialogue options based on quest state.
	 */
	UPROPERTY(EditDefaultsOnly, Instanced, BlueprintReadOnly, Category = "Dialogue|Node")
	TArray<TObjectPtr<UDialogueCondition>> EnterConditions{};

	/**
	 * @brief Static effects executed when node is entered. For example, to update quest state.
	 */
	UPROPERTY(EditDefaultsOnly, Instanced, BlueprintReadOnly, Category = "Dialogue|Node")
	TArray<TObjectPtr<UDialogueEffect>> EnterEffects{};

	/**
	 * @brief Player choices available at this node.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dialogue|Node")
	TArray<TObjectPtr<UDialogueChoiceDataAsset>> Choices{};
};

/**
 * @class UDialogueDefinition
 * @brief Dialogue definition containing nodes and choices.
 */
UCLASS(BlueprintType)
class DIALOGUE_API UDialogueDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	const FDialogueNodeDefinition* FindNodeById(FName NodeId) const;
	bool IsDefinitionValid() const;

private:
	// Helpers
	bool IsDefinitionСomplete() const;
	bool IsNodeValid(const FDialogueNodeDefinition& Node, TSet<FName>& NodeIds, const FName& InDialogueId) const;
	bool IsChoiceValid(const FDialogueNodeDefinition& Node) const;
	bool AreChoiceTargetsValid(const TSet<FName>& NodeIds) const;
public:
	/**
	 * @brief Unique dialogue id.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dialogue")
	FName DialogueId{NAME_None};

	/**
	 * @brief Used for UI
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dialogue")
	FText DisplayName{};

	/**
	 * @brief Entry node id. Dialogue session will start from this node.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dialogue")
	FName StartNodeId{NAME_None};

	/**
	 * @brief It is a NPC's phrase + optional player choices. Nodes must have unique NodeId values.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dialogue")
	TArray<FDialogueNodeDefinition> Nodes{};
};


/**
 * @struct FDialogueRuntimeContext
 * @brief Runtime context passed to dialogue conditions/effects.
 * Contains only transient runtime data, no persistent state.
 * 
 * @see UDialogueManagerSubsystem::BuildRuntimeContext
 */
struct DIALOGUE_API FDialogueRuntimeContext
{
	UWorld* World{nullptr};
	APawn* PlayerPawn{nullptr};
	AActor* NpcActor{nullptr};
	const UDialogueDefinition* DialogueDefinition{nullptr};
	FName CurrentNodeId{NAME_None};
	FName SelectedChoiceId{NAME_None};
};

/**
 * @class UDialogueChoiceDataAsset
 * @brief Data asset representing a player's choice in a dialogue node.
 */
UCLASS(BlueprintType)
class DIALOGUE_API UDialogueChoiceDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FDialogueChoiceDefinition Choice;
};

/**
 * @class UDialogueNodeDataAsset
 * @brief Data asset representing a dialogue node.
 */
UCLASS(BlueprintType)
class DIALOGUE_API UDialogueNodeDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FDialogueNodeDefinition Node;
};

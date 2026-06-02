 /*****************************************************************//**
  * \file   DialogueManagerSubsystem.h
  * \brief  World subsystem for dialogue runtime, validation, and progress.
  *
  * -----------------------------------------------------------------------------
  * HIGH-LEVEL FLOW
  * -----------------------------------------------------------------------------
  * 1) Startup:
  *    - Initialize() clears runtime maps.
  *    - LoadDialogueDefinitionsFromConfig() loads assets from config.
  *    - RegisterDialogueDefinition() validates and stores definitions by DialogueId.
  *
  * 2) Dialogue start:
  *    - External caller invokes TryStartDialogue(PlayerPawn, NpcActor, DialogueId, OutNode).
  *    - Subsystem checks authority, cooldown, limits, and definition existence.
  *    - Creates FDialogueSession (active runtime state).
  *    - EnterNode() applies enter conditions/effects and builds first FDialogueNodeRuntime.
  *    - Session is stored in ActiveSessionsByPlayer.
  *
  * 3) Player choice:
  *    - Caller invokes TrySelectChoice(PlayerPawn, SessionId, ChoiceId, OutNode).
  *    - Subsystem validates session, rate limit, selected choice, and conditions.
  *    - Applies choice effects, moves to next node via EnterNode(), or closes dialogue.
  *    - Returns updated FDialogueNodeRuntime to client/UI.
  *
  * 4) Runtime snapshot for UI:
  *    - BuildNodeRuntime() converts static node definition to runtime data:
  *      text + only available choices (after conditions).
  *    - FDialogueNodeRuntime is what client UI should render.
  *
  * 5) Close and cleanup:
  *    - TryCloseDialogue() removes active session for player.
  *    - CleanupInvalidSessions() removes stale/invalid sessions by timeout or invalid refs.
  *
  * -----------------------------------------------------------------------------
  * RELATIONSHIP SCHEME (SIMPLIFIED)
  * -----------------------------------------------------------------------------
  * [Dialogue Definitions]
  *        |
  *        v
  * UDialogueManagerSubsystem
  *   |-- DialogueDefinitionsById (static definitions)
  *   |-- ActiveSessionsByPlayer  (runtime sessions)
  *        |
  *        +--> TryStartDialogue / TrySelectChoice / TryCloseDialogue
  *               |
  *               +--> EnterNode -> BuildNodeRuntime -> FDialogueNodeRuntime (to UI)
  *
  * \date   May 2026
  *********************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"

#include "DialogueManagerSubsystem.generated.h"

class AActor;
class APawn;
class UDialogueCondition;
class UDialogueDefinition;
class UDialogueEffect;
struct FDialogueChoiceDefinition;
struct FDialogueNodeDefinition;
struct FDialogueRuntimeContext;
class APlayerState;
enum class EDialogueConditionMatchMode : uint8;

/**
 * @struct FDialogueChoiceRuntime
 * @brief Runtime data for one choice that UI can show.
 */
USTRUCT(BlueprintType)
struct FDialogueChoiceRuntime
{
	GENERATED_BODY()

	/**
	 * @see FDialogueChoiceDefinition::ChoiceId
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Dialogue|Runtime")
	FName ChoiceId{NAME_None};

	/**
	 * @brief Text shown to player for this choice.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Dialogue|Runtime")
	FText Text{};
};

/**
 * @struct FDialogueNodeRuntime
 * @brief Runtime snapshot of current node sent to client/UI.
 */
USTRUCT(BlueprintType)
struct FDialogueNodeRuntime
{
	GENERATED_BODY()

	/**
	 * @brief Active session id used to validate player choice requests.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Dialogue|Runtime")
	FGuid SessionId{};

	/**
	 * @brief  Id of dialogue definition used by this session.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Dialogue|Runtime")
	FName DialogueId{NAME_None};

	/**
	 * @brief Id of current node inside dialogue definition.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Dialogue|Runtime")
	FName NodeId{NAME_None};

	/**
	 * @brief speaker key for name/portrait in UI.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Dialogue|Runtime")
	FName SpeakerId{NAME_None};

	/**
	 * @brief Main node text shown to player.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Dialogue|Runtime")
	FText Text{};

	/**
	 * @brief List of currently available choices after condition checks.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Dialogue|Runtime")
	TArray<FDialogueChoiceRuntime> Choices{};

	/**
	 * @brief True when dialogue should be closed on client.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Dialogue|Runtime")
	bool bIsDialogueEnded{false};

	/**
	 * @brief Version number to detect outdated client snapshots. It protects against race conditions when player clicks a choice but the server already moved them to another node or ended the dialogue.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Dialogue|Runtime")
	int32 Revision{0};
};

/**
 * @struct FDialogueSession
 * @brief Active server-side dialogue state for one player.
 * @see UDialogueManagerSubsystem
 */
USTRUCT()
struct FDialogueSession
{
	GENERATED_BODY()
public:
	/**
	 * @brief Unique id of current active dialogue session.
	 */
	FGuid SessionId{};

	/**
	 * @brief Id of dialogue definition used by this session.
	 */
	FName DialogueId{NAME_None};

	/**
	 * @brief Current node id where player is now.
	 */
	FName CurrentNodeId{NAME_None};

	/**
	 * @brief Weak reference to player who owns this session.
	 */
	TWeakObjectPtr<APawn> PlayerPawn{};

	/**
	 * @brief Weak reference to NPC actor used in this dialogue.
	 */
	TWeakObjectPtr<AActor> NpcActor{};

	/**
	 * @brief Weak reference to loaded dialogue definition asset.
	 */
	TWeakObjectPtr<const UDialogueDefinition> Definition{};

	/**
	 * @brief Server-side session version, increased after state changes. 
	 */
	int32 Revision{0};

	/**
	 * @brief Last time this session was used on server.
	 */
	double LastActivityServerTime{0.0};

	/**
	 * @brief Last time player sent a choice request.
	 */
	double LastChoiceRequestServerTime{-1.0};
};


/**
 * @class UDialogueManagerSubsystem
 * @brief 
 */
UCLASS(Config = Game)
class DIALOGUE_API UDialogueManagerSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

						/* === C++ member functions === */
public:
	/**
	 * @brief  Prepare runtime maps and load dialogue assets when the world starts.
	 */
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/**
	 * @brief Clear all cached runtime data when the world shuts down.
	 */
	virtual void Deinitialize() override;

private:
	/**
	 * @brief Read startup dialogue asset list from config and register each valid asset.
	 */
	void LoadDialogueDefinitionsFromConfig();

	

// helpers

	///**
	// * @brief  Helper funtion.
	// * 
	// * Get player state from pawn for identity/progress usage.
	// */
	//APlayerState* GetOwningPlayerState(const APawn* PlayerPawn) const;

	/**
	 * @brief  Helper funtion.
	 * 
	 * Return current server world time for cooldown/rate checks.
	 */
	double GetServerTimeSeconds() const;

	const FDialogueChoiceDefinition* GetSelectedChoice(const FDialogueSession& Session, FName ChoiceId) const;


	/**
	 * @brief Helper funtion.
	 * 
	 * Generate a unique key for a player based on their pawn.
	 */
	static FName MakePlayerKey(const APawn* Pawn);

	/**
	 * @brief Helper funtion.
	 * 
	 * Remove stale sessions (invalid references or timeout) and clean related metadata.
	 */
	void CleanupInvalidSessions();

	/**
	 * @brief Helper funtion.
	 *
	 * Update last-choice time and block requests that are too frequent.
	 */
	bool TouchAndValidateChoiceRate(FDialogueSession& Session);

	/**
	 * @brief Helper funtion.
	 *
	 * Get read-only active session for this player.
	 */
	FDialogueSession* FindSession(const APawn* PlayerPawn);


// Logic helpers

	/**
	 * @brief Validate and add one dialogue definition to runtime registry.
	 */
	void RegisterDialogueDefinition(UDialogueDefinition* DialogueDefinition);

	/**
	 * @brief Helper funtion.
	 * 
	 * Move session to a new node, run enter checks/effects, and build runtime node data.
	 */
	bool EnterNode(FDialogueSession& Session, FName NextNodeId, FName SelectedChoiceId, FDialogueNodeRuntime& OutNode);

	/**
	 * @brief Helper funtion.
	 * 
	 * Convert static node data into filtered runtime data for UI/network.
	 */
	void BuildNodeRuntime(const FDialogueSession& Session, const FDialogueNodeDefinition& Node, FDialogueNodeRuntime& OutNode) const;

	/**
	 * @brief Helper funtion.
	 * 
	 * Evaluate conditions with AND/OR mode in current runtime context.
	 */
	bool AreConditionsSatisfied(const TArray<TObjectPtr<UDialogueCondition>>& Conditions, const FDialogueSession& Session,
		FName SelectedChoiceId, EDialogueConditionMatchMode MatchMode) const;

	/**
	 * @brief Helper funtion.
	 * 
	 * Execute all valid effects for current dialogue action.
	 */
	void ApplyEffects(const TArray<TObjectPtr<UDialogueEffect>>& Effects, const FDialogueSession& Session, FName SelectedChoiceId) const;

	/**
	 * @brief Helper funtion.
	 * 
	 * Build context object passed to condition and effect logic.
	 */
	FDialogueRuntimeContext BuildRuntimeContext(const FDialogueSession& Session, FName SelectedChoiceId) const;



						/* === C++ member variables === */
private:
	/**
	 * @brief  Runtime map of active dialogue sessions, one entry per player key.
	 * FName - Player key generated from pawn reference for identity and progress tracking.
	 * FDialogueSession - Runtime state of active dialogue session
	 */
	TMap<FName, FDialogueSession> ActiveSessionsByPlayer{};

	


						/* === Unreal Engine UFUNCTION === */
public:

	/**
	 * @brief Find a dialogue definition by id in runtime registry.
	 */
	UFUNCTION(BlueprintPure, Category = "Dialogue|Manager")
	const UDialogueDefinition* GetDialogueDefinition(FName DialogueId) const;

	/**
	 * @brief Start new dialogue session on server and return first runtime node.
	 * @see  ANpcBase::TryStartDialogueDirect
	 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Dialogue|Runtime")
	bool TryStartDialogue(APawn* PlayerPawn, AActor* NpcActor, FName DialogueId, FDialogueNodeRuntime& OutNode);

	/**
	 * @brief Validate and apply player choice, then move to next node or end.
	 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Dialogue|Runtime")
	bool TrySelectChoice(APawn* PlayerPawn, FGuid SessionId, FName ChoiceId, FDialogueNodeRuntime& OutNode);

	/**
	 * @brief  Force close current active dialogue session for player.
	 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Dialogue|Runtime")
	bool TryCloseDialogue(APawn* PlayerPawn);

	/**
	 * @brief Return current runtime node of active session, if any.
	 */
	UFUNCTION(BlueprintPure, Category = "Dialogue|Runtime")
	bool GetActiveDialogueNode(const APawn* PlayerPawn, FDialogueNodeRuntime& OutNode);


						/* === Unreal Engine UPROPERTY === */
protected:
	/**
	 * @brief Minimum delay between choice requests to protect from spam.
	 */
	UPROPERTY(Config, EditAnywhere, Category = "Dialogue|Runtime", meta = (ClampMin = "0.01"))
	float MinChoiceRequestIntervalSeconds{0.05f};

	/**
	 * @brief Time limit after which inactive session is removed.
	 */
	UPROPERTY(Config, EditAnywhere, Category = "Dialogue|Runtime", meta = (ClampMin = "5.0"))
	float SessionIdleTimeoutSeconds{120.0f};

	/**
	 * @brief Hard limit for active sessions in this world.
	 */
	UPROPERTY(Config, EditAnywhere, Category = "Dialogue|Runtime", meta = (ClampMin = "1"))
	int32 MaxActiveSessions{2000};

private:
	/**
	 * @brief Fast lookup table for loaded dialogue definitions by id.
	 */
	UPROPERTY()
	TMap<FName, TObjectPtr<UDialogueDefinition>> DialogueDefinitionsById{};

	/**
	 * @brief Config list of dialogue assets to load at subsystem startup.
	 */
	UPROPERTY(Config, EditAnywhere, Category = "Dialogue|Manager")
	TArray<TSoftObjectPtr<UDialogueDefinition>> StartupDialogueDefinitions{};
};
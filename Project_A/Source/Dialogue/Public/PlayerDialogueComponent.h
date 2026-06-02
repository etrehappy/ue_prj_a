/*****************************************************************//**
 * \file   PlayerDialogueComponent.h
 * \brief  Default component used by player characters.
 * 
 * \date   May 2026
 *********************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DialogueManagerSubsystem.h"

#include "PlayerDialogueComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnDialogueNodeReceived, const FDialogueNodeRuntime& /*Node*/);
DECLARE_MULTICAST_DELEGATE(FOnDialogueClosed);

/**
 *	@class UPlayerDialogueComponent
 *  @brief Used by player characters to interact with the dialogue system. This component is responsible for sending dialogue requests to the server and receiving dialogue updates from the server.
 *	@see UDialogueManagerSubsystem
 *	@see NetPlayerCharacter
 *	@see GeneralHud
 */
UCLASS(ClassGroup = (Dialogue), meta = (BlueprintSpawnableComponent))
class DIALOGUE_API UPlayerDialogueComponent : public UActorComponent
{
	GENERATED_BODY()

						/* === C++ member functions === */
public:
	UPlayerDialogueComponent();
	virtual ~UPlayerDialogueComponent() = default;

	/**
	 * @brief Client function.
	 * 
	 * @see ANetPlayerCharacter::Interact
	 * @see Server_RequestDialogueSnapshot
	 */
	void RequestDialogueSnapshot();

	/**
	 * @brief Client function.
	 *
	 *	It submits the player's dialogue choice to the server. 
	 * 
	 *  @see Server_SubmitDialogueChoice
	 *	@see AGeneralHud::SubmitDialogueChoice
	 *  @param[in] SessionId The unique identifier for the dialogue session.
	 *	@param[in] ChoiceId The identifier for the player's chosen dialogue option.
	 */
	void SubmitDialogueChoice(FGuid SessionId, FName ChoiceId);

	/**
	 * @brief Client function.
	 *
	 * @see Server_CloseDialogue
	 */
	void CloseDialogue();

private:
	APawn* GetOwnerPawn() const;
	UDialogueManagerSubsystem* GetDialogueSubsystem(APawn* PlayerPawn) const;


						/* === C++ member variables === */
public:
	FOnDialogueNodeReceived OnDialogueNodeReceived{};
	FOnDialogueClosed OnDialogueClosed{};


						/* === Unreal Engine UFUNCTION === */
protected:
	/**
	 * @brief The server will respond with a dialogue node if there is an active dialogue, or will respond with a dialogue closed message if there is no active dialogue.
	 * 
	 * @see Client_DialogueNodeAvailable
	 */
	UFUNCTION(Server, Reliable)
	void Server_RequestDialogueSnapshot();

	/**
	 * @brief It closes the active dialogue on the server. The server will respond with a dialogue closed message.
	 */
	UFUNCTION(Server, Reliable)
	void Server_CloseDialogue();

	/**
	 * @see ANetPlayerCharacter
	 */
	UFUNCTION(Client, Reliable)
	void Client_DialogueNodeAvailable(const FDialogueNodeRuntime& Node);

	/**
	 * @see ANetPlayerCharacter
	 */
	UFUNCTION(Client, Reliable)
	void Client_DialogueClosed();

	/**
	 * @brief The server will respond with a dialogue node if the choice is valid and there is an active dialogue, 
	 *	or will respond with a dialogue closed message if the choice is invalid or there is no active dialogue.
	 * 
	 * @param[in] SessionId The unique identifier for the dialogue session.
	 * @param[in] ChoiceId The identifier for the player's chosen dialogue option.
	 */
	UFUNCTION(Server, Reliable)
	void Server_SubmitDialogueChoice(FGuid SessionId, FName ChoiceId);
};
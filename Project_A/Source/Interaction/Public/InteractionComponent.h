/*****************************************************************//**
 * \file   InteractionComponent.h
 * \brief  A component for characters to detect interactable actors.
 * 
 * \date   January 2026
 *********************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractionComponent.generated.h"

class IInteractable;

/**
 * @param NewActor The new actor that has become the focus of interaction.
 */
DECLARE_MULTICAST_DELEGATE_OneParam(FOnInteractableFocusChanged, AActor* /*NewActor*/);

/**
 * @class UInteractionComponent
 * @brief Component that allows characters to detect and interact with interactable actors.
 */
UCLASS( ClassGroup=(CustomInteraction), meta=(BlueprintSpawnableComponent) )
class INTERACTION_API UInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

						/* === C++ member functions === */
public:	
	UInteractionComponent();
	virtual ~UInteractionComponent() override = default;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	/**
	 * @brief Server function.
	 * 
	 * Finds the best candidate and triggers the `OnFocusChanged` event.
	 */
	void UpdateInteractionFocus();

	/**
	 * @brief Client function.
	 * 
	 * Calls the server function.
	 */
	void TryInteract();

	/**
	 * @brief Server function.
	 * 
	 * Adds an actor to the set of candidates for interaction.
	 * @param[in] OtherActor The actor that has begun overlapping with the character.
	 */
	void OtherBeginOverlap(AActor* OtherActor);

	/**
	 * @brief Server function.
	 * 
	 * Removes an actor from the set of candidates for interaction.
	 * @param[in] OtherActor The actor that has ended overlapping with the character.
	 */
	void OtherEndOverlap(AActor* OtherActor);	

protected:	
	virtual void BeginPlay() override;

private:
	/**
	 * @brief Server function.
	 * 
	 * Finds the best candidate for interaction from the set of candidates.	 * 
	 * @return The actor that is the best candidate for interaction, or nullptr if no suitable candidate is found.
	 */
	AActor* FindBestCandidate();

	/* Helper functions */
	APawn* GetInteractor() const;
	IInteractable* GetFocusedInteractable(APawn* Interactor, AActor*& OutActor) const;
	bool IsWithinInteractionRange(APawn* Interactor, AActor* Target, IInteractable* Interactable) const;



						/* === C++ member variables === */
public:
	FOnInteractableFocusChanged OnFocusChanged{};

protected:
	/**
	 * @brief Actors that are candidates for interaction.
	 */
	TSet<TWeakObjectPtr<AActor>> Candidates{};


						/* === Unreal Engine UFUNCTIONS === */
protected:
	UFUNCTION(Server, Reliable)
	void Server_TryInteract();


						/* === Unreal Engine UPROPERTY === */
protected:
	/**
	 * @brief The actor that is currently focused for interaction.
	 */
	UPROPERTY()
	TWeakObjectPtr<AActor> FocusedActor{};
	

						/* === Additional === */	
protected:
	/**
	 * @brief Debug function to show interaction feedback.
	 */
	UFUNCTION(Client, Unreliable)
	void Client_ShowInteractionFeedback(const FVector Origin, const FVector Direction, const float Distance, const float AngleRad) const;
	
};

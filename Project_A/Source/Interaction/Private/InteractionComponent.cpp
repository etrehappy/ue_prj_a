


#include "InteractionComponent.h"
#include "Interactable.h"

#include "ProjectALog.h"


UInteractionComponent::UInteractionComponent()
{	
	PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.TickInterval = 0.2f;
    SetIsReplicatedByDefault(false);
}

void UInteractionComponent::BeginPlay()
{
	Super::BeginPlay();	
}

void UInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (IsRunningDedicatedServer())
    {
        UpdateInteractionFocus();
    }
}

void UInteractionComponent::UpdateInteractionFocus()
{
    FocusedActor = FindBestCandidate();
    OnFocusChanged.Broadcast(FocusedActor.Get());
}

void UInteractionComponent::TryInteract()
{
    Server_TryInteract();
}

void UInteractionComponent::Server_TryInteract_Implementation()
{
    APawn* Interactor = GetInteractor();
    if (!Interactor)
    {
		UE_LOGFMT(LogProjectA, Warning, "{0} - Interactor is empty", FString(__FUNCTION__));
        return;
    }

    AActor* BestCandidate = nullptr;
    IInteractable* Interactable = GetFocusedInteractable(Interactor, BestCandidate);
    if (!Interactable)
    {
		UE_LOGFMT(LogProjectA, Warning, "{0} - No interactable focused", FString(__FUNCTION__));
        return;
    }

    if (!IsWithinInteractionRange(Interactor, BestCandidate, Interactable))
    {
		UE_LOGFMT(LogProjectA, Warning, "{0} - Not within interaction range of {1}", FString(__FUNCTION__), *BestCandidate->GetName());
        return;
    }

    Server_RequestInteraction(BestCandidate);
    //Interactable->Interact(Interactor);
}

APawn* UInteractionComponent::GetInteractor() const
{
    return Cast<APawn>(GetOwner());
}

IInteractable* UInteractionComponent::GetFocusedInteractable(APawn* Interactor, AActor*& OutActor) const
{
    OutActor = nullptr;

    if (!Interactor)
    {
        UE_LOGFMT(LogProjectA, Warning, "{0} - Interactor is empty", FString(__FUNCTION__));
        return nullptr;
    }

    OutActor = FocusedActor.Get();
    if (!OutActor)
    {
        UE_LOGFMT(LogProjectA, Warning, "{0} - BestCandidate is empty", FString(__FUNCTION__));
        return nullptr;
    }

    IInteractable* Interactable = Cast<IInteractable>(OutActor);
    if (!Interactable)
    {
        UE_LOGFMT(LogProjectA, Warning, "{0} - Interactable is empty", FString(__FUNCTION__));
        return nullptr;
    }

    if (!Interactable->CanInteract(Interactor))
    {
        UE_LOGFMT(LogProjectA, Warning, "{0} - Can't interact with {1}", FString(__FUNCTION__), *OutActor->GetName());
        return nullptr;
    }

    return Interactable;
}

bool UInteractionComponent::IsWithinInteractionRange(APawn* Interactor, AActor* Target, IInteractable* Interactable) const
{
    // Server validation
    const float DistanceToInteractableActor = FVector::Dist(Interactor->GetActorLocation(), Target->GetActorLocation());

    const FVector InteractorForwardVector = Interactor->GetController() ? Interactor->GetController()->GetControlRotation().Vector() : Interactor->GetActorForwardVector();

#if WITH_EDITOR
    Client_ShowInteractionFeedback(Interactor->GetActorLocation(), InteractorForwardVector, DistanceToInteractableActor, FMath::DegreesToRadians(Interactable->GetInteractionAngle()));
#endif // WITH_EDITOR

    if (DistanceToInteractableActor > Interactable->GetInteractionDistance())
    {
        UE_LOGFMT(LogProjectA, Warning, "{0} - Too far to interact with {1}", FString(__FUNCTION__), *Target->GetName());
        return false;
    }

    FVector ToTargetDirection = (Target->GetActorLocation() - Interactor->GetActorLocation()).GetSafeNormal();
    float FacingDot = FVector::DotProduct(InteractorForwardVector, ToTargetDirection); // how much the interactor “looks” at the target

    if (FacingDot < FMath::Cos(FMath::DegreesToRadians(Interactable->GetInteractionAngle())))
    {
        UE_LOGFMT(LogProjectA, Warning, "{0} - Not looking at {1}", FString(__FUNCTION__), *Target->GetName());
        return false;
    }

    return true;
}

void UInteractionComponent::Server_SubmitInteractionAction_Implementation(AActor* TargetActor, FGameplayTag ActionId)
{
    if (!TargetActor || !ActionId.IsValid())
    {
        UE_LOGFMT(LogProjectA, Warning, "{0} - Invalid TargetActor or ActionId", FString(__FUNCTION__));
        return;
    }

    APawn* Interactor = GetInteractor();
    if (!Interactor)
    {
        UE_LOGFMT(LogProjectA, Warning, "{0} - Interactor is empty", FString(__FUNCTION__));
        return;
    }

    if (FocusedActor.Get() != TargetActor)
    {
        UE_LOGFMT(LogProjectA, Warning, "{0} - TargetActor is not focused", FString(__FUNCTION__));
        return;
    }

    IInteractable* Interactable = Cast<IInteractable>(TargetActor);
    if (!Interactable)
    {
        UE_LOGFMT(LogProjectA, Warning, "{0} - TargetActor is not interactable", FString(__FUNCTION__));
        return;
    }

    if (!Interactable->CanInteract(Interactor))
    {
        UE_LOGFMT(LogProjectA, Warning, "{0} - Can't interact with {1}", FString(__FUNCTION__), *TargetActor->GetName());
        return;
    }

    if (!IsWithinInteractionRange(Interactor, TargetActor, Interactable))
    {
        UE_LOGFMT(LogProjectA, Warning, "{0} - Not within interaction range of {1}", FString(__FUNCTION__), *TargetActor->GetName());
        return;
    }

    TArray<FInteractionActionType> Actions{};
    Interactable->BuildInteractionActions(Interactor, Actions);

    bool bActionAllowed = false;
    for (const FInteractionActionType& Action : Actions)
    {
        if (Action.Definition->ActionTag.MatchesTagExact(ActionId))
        {
            bActionAllowed = Action.bIsEnabled;
            break;
        }
    }

    if (!bActionAllowed)
    {
        UE_LOGFMT(LogProjectA, Warning, "{0} - Action is not allowed: {1}", FString(__FUNCTION__), ActionId.ToString());
        return;
    }

    Interactable->ExecuteInteractionAction(Interactor, ActionId);
}

void UInteractionComponent::Client_InteractionAvailable_Implementation(AActor* TargetActor, const TArray<FInteractionActionType>& Actions)
{
    if (!TargetActor)
    {
        UE_LOGFMT(LogProjectA, Warning, "{0} - TargetActor is empty", FString(__FUNCTION__));
        return;
    }

    OnInteractionReceived.Broadcast(TargetActor, Actions);
}

void UInteractionComponent::Server_RequestInteraction_Implementation(AActor* TargetActor)
{

    APawn* Interactor = GetInteractor();
    if (!Interactor)
    {
        UE_LOGFMT(LogProjectA, Warning, "{0} - Interactor is empty", FString(__FUNCTION__));
        return;
    }

    if (!TargetActor)
    {
        UE_LOGFMT(LogProjectA, Warning, "{0} - Invalid parameters TargetActor", FString(__FUNCTION__));
		return;
    }

    IInteractable* Interactable = Cast<IInteractable>(TargetActor);
    if (!Interactable)
    {
        UE_LOGFMT(LogProjectA, Warning, "{0} - TargetActor is not interactable: {1}", FString(__FUNCTION__), *TargetActor->GetName());
        return;
    }

    if (!Interactable->CanInteract(Interactor))
    {
        UE_LOGFMT(LogProjectA, Warning, "{0} - Can't interact with {1}", FString(__FUNCTION__), *TargetActor->GetName());
        return;
    }

    if (!IsWithinInteractionRange(Interactor, TargetActor, Interactable))
    {
        UE_LOGFMT(LogProjectA, Warning, "{0} - Not within interaction range of {1}", FString(__FUNCTION__), *TargetActor->GetName());
        return;
    }


    TArray<FInteractionActionType> Actions{};
    Interactable->BuildInteractionActions(Interactor, Actions);

    if (Actions.Num() == 0)
    {
        UE_LOGFMT(LogProjectA, Warning, "{0} - No interaction actions available for {1}", FString(__FUNCTION__), *TargetActor->GetName());
        return;
	}    

    Client_InteractionAvailable(TargetActor, Actions);
}

void UInteractionComponent::OtherBeginOverlap(AActor* OtherActor)
{	
    //UE_LOGFMT(LogProjectA, Log, "{0} - is called", FString(__FUNCTION__));

    if (!OtherActor)
    {
        UE_LOGFMT(LogProjectA, Warning, "{0} - OtherBeginOverlap called with null OtherActor", FString(__FUNCTION__));
        return;
    }
    
    if (OtherActor->Implements<UInteractable>())
    {
		//UE_LOGFMT(LogProjectA, Log, "{0} - Actor implements UInteractable: {1}", FString(__FUNCTION__), *OtherActor->GetName());
        Candidates.Add(OtherActor);
    }
    else
    {
        UE_LOGFMT(LogProjectA, Verbose, "{0} - Actor does NOT implement UInteractable: {1}", FString(__FUNCTION__), *OtherActor->GetName());
    }

}

void UInteractionComponent::OtherEndOverlap(AActor* OtherActor)
{
    //UE_LOGFMT(LogProjectA, Log, "{0} - is called", FString(__FUNCTION__));

	Candidates.Remove(OtherActor);
}


void UInteractionComponent::SubmitInteractionAction(AActor* TargetActor, FGameplayTag ActionId)
{
    Server_SubmitInteractionAction(TargetActor, ActionId);
}

AActor* UInteractionComponent::FindBestCandidate()
{   
	APawn* Interactor = GetInteractor();
    if(!Interactor)
    {
        UE_LOGFMT(LogProjectA, Warning, "{0} - Interactor is empty", FString(__FUNCTION__));
        return nullptr;
    }

    AActor* Best = nullptr;

    // This is the best score among all candidates. 
    // Set the minimum possible value so that the first valid candidate would be the 'best'.
    float BestScore = -FLT_MAX;

    const FVector InteractorForwardVector = Interactor->GetController() ? Interactor->GetController()->GetControlRotation().Vector() : Interactor->GetActorForwardVector();
    const FVector InteractorLocation = Interactor->GetActorLocation();

    for (auto& Ptr : Candidates)
    {
        AActor* Candidate = Ptr.Get();
        if (!Candidate) continue;
        if (Candidate == Interactor)
        {
			//UE_LOGFMT(LogProjectA, Log, "{0} - Skipping self candidate", FString(__FUNCTION__));
            continue;
        }
        
        // Normalised direction from Interactor to candidate.
        const FVector Direction = (Candidate->GetActorLocation() - InteractorLocation).GetSafeNormal();
        
        // The scalar product between the direction of gaze and the direction towards the actor.
        // Shows how far ahead the object is: 1 — directly in front of the line of sight, 0 — to the side, -1 — behind.
        float Dot = FVector::DotProduct(InteractorForwardVector, Direction);

        // Distance to candidate.
        float Dist = FVector::Dist(InteractorLocation, Candidate->GetActorLocation());

        // The larger 'Dot', the better, as the object is closer to the centre of vision. 
        // The smaller 'Dist', the better, as the object is physically closer. 
        // 2.f and 0.01f - empirical weighting coefficients.
        float Score = Dot * 2.f - Dist * 0.01f;

        //UE_LOGFMT(LogProjectA, Verbose, "{0} - Candidate {1}: Dot={2:.3f}, Dist={3:.1f}, Score={4:.3f}", FString(__FUNCTION__), *Candidate->GetName(), Dot, Dist, Score);

        if (Score > BestScore)
        {
            BestScore = Score;
            Best = Candidate;
        }
    }

    return Best;
}




void UInteractionComponent::Client_ShowInteractionFeedback_Implementation(const FVector Origin, const FVector Direction, const float Distance, const float AngleRad) const
{
#if WITH_EDITOR    
    UWorld* World = GetWorld();
    if (!World) return;

    DrawDebugCone(World, Origin, Direction, Distance, AngleRad, AngleRad, 16, FColor::Cyan, false, 1.f, 0, 2.f);
#endif // WITH_EDITOR
}
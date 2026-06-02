#include "ItemPickup.h"
#include "Components/SphereComponent.h"
#include "PickUpInterface.h"

#include "ProjectALog.h"

AItemPickup::AItemPickup()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	SetReplicateMovement(true);

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AItemPickup::OtherBeginOverlap);
	SphereComponent->OnComponentEndOverlap.AddDynamic(this, &AItemPickup::OtherEndOverlap);	

	SetRootComponent(SphereComponent);

	SphereComponent->SetSphereRadius(100.f);
	SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	SphereComponent->SetCollisionObjectType(ECC_WorldDynamic);
	SphereComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));	
	StaticMeshComponent->SetupAttachment(GetRootComponent());
	SphereComponent->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	SphereComponent->SetCollisionObjectType(ECC_WorldDynamic);
	SphereComponent->SetCollisionResponseToAllChannels(ECR_Block);
	SphereComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
}

void AItemPickup::BeginPlay()
{
	Super::BeginPlay();
	
}


void AItemPickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool AItemPickup::CanInteract(APawn* Interactor) const
{
	return true;
}

//void AItemPickup::Interact(APawn* Interactor)
//{
//	UE_LOGFMT(LogProjectA, Log, "{0} - called", FString(__FUNCTION__));
//
//	if(!Interactor)
//	{
//		UE_LOGFMT(LogProjectA, Warning, "{0} - Interactor is nullptr", FString(__FUNCTION__));
//		return;
//	}
//
//	PickUp(Interactor);
//}

void AItemPickup::PickUp(APawn* Picker)
{
	UE_LOGFMT(LogProjectA, Log, "{0} - called", FString(__FUNCTION__));

	if (!Picker)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Picker is nullptr", FString(__FUNCTION__));
		return;
	}
	
	if (!Picker->Implements<UPickUpInterface>())
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - {1} has not IPickUpInterface", FString(__FUNCTION__), Picker->GetName());
		return;
	}

	bool bIsPickedUp = IPickUpInterface::Execute_PickUpItem(Picker, this);

	if (bIsPickedUp)
	{
		Destroy();
	}	
}

void AItemPickup::OtherBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

}

void AItemPickup::OtherEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{	
}

void AItemPickup::BuildInteractionActions(APawn* Interactor, TArray<FInteractionActionType>& OutActions) const
{
	OutActions.Reset();

	if (!CanInteract(Interactor))
	{
		return;
	}

	for (const TObjectPtr<UInteractionActionDefinition>& ActionDef : InteractionActions)
	{
		if (!ActionDef || !ActionDef->ActionTag.IsValid())
		{
			continue;
		}

		FInteractionActionType ActionView{};	
		ActionView.bIsEnabled = ActionDef->bEnabledByDefault; 
		ActionView.Definition = ActionDef;

		OutActions.Add(ActionView);
	}
}

bool AItemPickup::ExecuteInteractionAction(APawn* Interactor, FGameplayTag ActionTag)
{
	if (!CanInteract(Interactor) || !ActionTag.IsValid())
	{
		return false;
	}

	// 1. Check if the action tag matches any of the defined interaction actions.
	const UInteractionActionDefinition* MatchedActionDef = nullptr;

	for (const TObjectPtr<UInteractionActionDefinition>& ActionDef : InteractionActions)
	{
		if (!ActionDef || !ActionDef->ActionTag.IsValid())
		{
			continue;
		}

		if (ActionDef->ActionTag.MatchesTagExact(ActionTag))
		{
			MatchedActionDef = ActionDef;
			break;
		}
	}

	if (!MatchedActionDef || !MatchedActionDef->bEnabledByDefault)
	{
		return false;
	}

	return BP_ExecuteInteractionAction(Interactor, ActionTag);
}
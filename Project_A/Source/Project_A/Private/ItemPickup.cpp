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

void AItemPickup::PickUp(AActor* Picker)
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

	IPickUpInterface::Execute_PickUpItem(Picker, this);

	Destroy();
}

void AItemPickup::OtherBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor->Implements<UPickUpInterface>())
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - {1} has not IPickUpInterface", FString(__FUNCTION__), OtherActor->GetName());
		return;
	}

	if (IPickUpInterface* Interface = Cast<IPickUpInterface>(OtherActor)) 
	{ 
		Interface->UpdatedCurrentPickUpItem(this); 
	}	
}

void AItemPickup::OtherEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{	
	if (!OtherActor->Implements<UPickUpInterface>())
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - {1} has not IPickUpInterface", FString(__FUNCTION__), OtherActor->GetName());
		return;
	}

	if (IPickUpInterface* Interface = Cast<IPickUpInterface>(OtherActor))
	{
		Interface->CleanCurrentPickUpItem();
	}
}






#include "ActorForTestNull.h"


// Sets default values
AActorForTestNull::AActorForTestNull()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AActorForTestNull::BeginPlay()
{
	Super::BeginPlay();
	
}

FVector AActorForTestNull::GetTargetActorLocation() const
{
	if (!AnyActor)
	{
		return FVector::ZeroVector;
	}
	
	return AnyActor->GetActorLocation();
}


//// Called every frame
//void AActorForTestNull::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//
//}




#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/ShapeComponent.h"

#include "BasePawn.generated.h"

UCLASS()
class PROJECT_A_API ABasePawn : public APawn
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABasePawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	
};

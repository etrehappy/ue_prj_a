// Copyright Epic Games, Inc. All Rights Reserved.


#include "Character/CustomPlayerController.h"
#include "Engine/LocalPlayer.h"
#include "EnhancedInputSubsystems.h"
#include "GameInstanceBase.h"
#include "HubGameMode.h"
#include "EnemyFactory.h"

#include "ProjectALog.h"


void ACustomPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (!IsLocalController() ) { return; }

	// Add Input Mapping Contexts
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{

		for (const FInputMappingContextWithPriority& Item : InputMappingContext)
		{
			if (!Item.MappingContext)
			{
				UE_LOGFMT(LogProjectA, Warning, "{0} - DefaultMappingContexts contains a null entry", FString(__FUNCTION__));              
				continue;
			}
			
			Subsystem->AddMappingContext(Item.MappingContext, Item.Priority);
		}
	}
}

void ACustomPlayerController::Client_OnConnected_Implementation(EServerWorldType World)
{    
	UE_LOGFMT(LogProjectA, Log, "{0} - Connected to the server successfully", FString(__FUNCTION__));

	UGameInstanceBase* GameInstanceP = GetGameInstance<UGameInstanceBase>();
	if (!GameInstanceP)
	{   
		UE_LOGFMT(LogProjectA, Warning, "{0} -  GameInstance is not found", FString(__FUNCTION__));
		return;
	};

	GameInstanceP->OnConnectedSuccessfully(World);
}

void ACustomPlayerController::Server_RequestEnterToWorld_Implementation()
{   
	AHubGameMode* GameModeP = GetWorld()->GetAuthGameMode<AHubGameMode>();
	if (!GameModeP) 
	{       
		UE_LOGFMT(LogProjectA, Warning, "{0} -  GameModeP is not found", FString(__FUNCTION__));
		return;
	};    

	GameModeP->EnterToWorld(this);
}


#if UE_BUILD_DEVELOPMENT

namespace
{
	bool GetEnemyLocationRotation(const APawn* PlayerPawn, FVector& GroundLocation, FRotator& FacingRotation)
	{
		float CapsuleRadius = 22.f;
		float CapsuleHalfHeight = 70.f;
		FVector SpawnPoint = PlayerPawn->GetActorLocation() + PlayerPawn->GetActorForwardVector() * 200.f;

		//Sweep
		FVector Start = SpawnPoint + FVector(0, 0, 5000);
		FVector End = SpawnPoint - FVector(0, 0, 5000);
		FHitResult Hit;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(PlayerPawn);
		bool bHit = PlayerPawn->GetWorld()->SweepSingleByChannel(Hit, Start, End,
			FQuat::Identity, ECC_Visibility,
			FCollisionShape::MakeCapsule(CapsuleRadius, CapsuleHalfHeight),
			Params
		);

		if (!bHit)
		{
			UE_LOGFMT(LogProjectA, Warning, "{0} -  Sweep did not hit any surface", FString(__FUNCTION__));
			return false;
		}

		GroundLocation = Hit.Location;

		FVector DirToPlayer = PlayerPawn->GetActorLocation() - GroundLocation;
		DirToPlayer.Z = 0; //without tilt
		FacingRotation = DirToPlayer.Rotation();

		return true;
	}
}
#endif //UE_BUILD_DEVELOPMENT

void ACustomPlayerController::Server_SpawnEnemyInFront_Implementation(UDataTable* EnemyDataTable, FGameplayTag EnemyTag)
{

#if UE_BUILD_DEVELOPMENT

	APawn* PlayerPawn = GetPawn();
	if (!PlayerPawn)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} -  PlayerPawn is not found", FString(__FUNCTION__));
		return;
	};

	UEnemyFactory* EnemyFactory = NewObject<UEnemyFactory>();
	if (!EnemyFactory)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} -  EnemyFactory is not created", FString(__FUNCTION__));
		return;
	};

	FVector GroundLocation{};
	FRotator FacingRotation{};

	if(! ::GetEnemyLocationRotation(PlayerPawn, GroundLocation, FacingRotation) )
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} -  Could not get spawn location and rotation", FString(__FUNCTION__));
		return;
	}

	Client_DrawEnemySpawnDebug(GroundLocation, FacingRotation);

	EnemyFactory->SpawnEnemyByTag(GetWorld(), EnemyDataTable, EnemyTag, GroundLocation, FacingRotation);

#endif //UE_BUILD_DEVELOPMENT
}

void ACustomPlayerController::Client_DrawEnemySpawnDebug_Implementation(FVector Location, FRotator Rotation)
{
#if UE_BUILD_DEVELOPMENT
	if (!IsLocalController()) { return; }

	UWorld* World = GetWorld();
	if (!World) { return; }

	FVector Start = Location + FVector(0, 0, 5000);
	FVector End = Location - FVector(0, 0, 5000);

	DrawDebugLine(World, Start, End, FColor::Blue, false, 5.0f);
	DrawDebugSphere(World, Location, 25, 12, FColor::Red, false, 5.0f);
#endif //UE_BUILD_DEVELOPMENT
}



///////////////////////////////////////////////////////////////////////////////
//AutoTests

#if WITH_DEV_AUTOMATION_TESTS

const TArray<FInputMappingContextWithPriority>& ACustomPlayerController::AutoTestGetInputMappingContext() const
{    
	return InputMappingContext;
}

#endif //WITH_DEV_AUTOMATION_TESTS


//void ACustomPlayerController::OnPossess(APawn* APawn)
//{
//	Super::OnPossess(APawn);
//
//	UE_LOGFMT(LogProjectA, Log, "{0} is executed: PlayerControllerName - {1}, PawnName - {2} ", FString(__FUNCTION__), *GetNameSafe(this), *GetNameSafe(APawn));
//}
//
//void ACustomPlayerController::OnUnPossess()
//{
//	Super::OnUnPossess();
//
//}

// Copyright Epic Games, Inc. All Rights Reserved.


#include "Character/CustomPlayerController.h"
#include "Engine/LocalPlayer.h"
#include "EnhancedInputSubsystems.h"
#include "Core/GameInstanceBase.h"
#include "Core/HubGameMode.h"
#include "EnemyFactory.h"
#include "GeneralHud.h"
#include "GameFramework/GameModeBase.h"
#include "Kismet/KismetSystemLibrary.h"
#include "DeathMenuWidget.h"
#include "BaseHud.h"

#include "ProjectALog.h"



void ACustomPlayerController::BeginPlay()
{
	Super::BeginPlay();
}

void ACustomPlayerController::TryInitialiseHudForPawn()
{
	if (!IsLocalController())
	{
		return;
	}

	if (AGeneralHud* HUD = Cast<AGeneralHud>(GetHUD()))
	{
		HUD->InitialiseMainWidgetForPawn();
	}
}

void ACustomPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	TryInitialiseHudForPawn();

	SetIgnoreMoveInput(false);
	SetIgnoreLookInput(false);
	bRespawnRequested = false;
}

void ACustomPlayerController::AcknowledgePossession(APawn* P)
{
	Super::AcknowledgePossession(P);
	TryInitialiseHudForPawn();
}

void ACustomPlayerController::OnRep_Pawn()
{
	Super::OnRep_Pawn();
	TryInitialiseHudForPawn();
}


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

	ABaseHud* BaseHud = Cast<ABaseHud>(GetHUD());
	if (!BaseHud)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - BaseHud is not found", FString(__FUNCTION__));
		return;
	}

	BaseHud->OnConnectedSuccessfully(World);
	
	if (World == EServerWorldType::Hub)
	{
		Server_RequestWorldServerList();
	}
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


void ACustomPlayerController::OnFocusChanged(AActor* NewFocusedActor)
{
	Client_OnFocusChanged(NewFocusedActor);
}

void ACustomPlayerController::Client_OnFocusChanged_Implementation(AActor* NewFocusedActor)
{
	if (!NewFocusedActor)
	{
		HidePopup();
		return;
	}

	ShowPopup(NewFocusedActor);
}

void ACustomPlayerController::Server_RequestRespawn_Implementation()
{
	if (bRespawnRequested)
	{
		return;
	}
	
	AGameModeBase* GameMode = GetWorld() ? GetWorld()->GetAuthGameMode() : nullptr;
	if (!GameMode) 
	{ 
		UE_LOGFMT(LogProjectA, Warning, "{0} -  GameMode is not found", FString(__FUNCTION__));
		return; 
	}

	bRespawnRequested = true;
	APawn* DeadPawn = GetPawn();

	if (!DeadPawn)
	{
		UE_LOGFMT(LogProjectA, Log, "{0} -  DeadPawn is not found", FString(__FUNCTION__));
		bRespawnRequested = false;
		return;
	}

	DeadPawn->SetActorHiddenInGame(true);
	DeadPawn->SetActorEnableCollision(false);
	DeadPawn->DetachFromControllerPendingDestroy();
	DeadPawn->Destroy();
	
	GameMode->RestartPlayer(this);
}

void ACustomPlayerController::ShowDeathMenu()
{
	if (!IsLocalController())
	{
		return;
	}

	if (!DeathMenuWidget)
	{
		if (!DeathMenuWidgetClass)
		{
			UE_LOGFMT(LogProjectA, Warning, "{0} - DeathMenuWidgetClass is not set", FString(__FUNCTION__));
			return;
		}

		DeathMenuWidget = CreateWidget<UDeathMenuWidget>(this, DeathMenuWidgetClass);
		if (!DeathMenuWidget)
		{
			UE_LOGFMT(LogProjectA, Warning, "{0} - Failed to create DeathMenuWidget", FString(__FUNCTION__));
			return;
		}

		DeathMenuWidget->OnRespawnRequested.AddDynamic(this, &ACustomPlayerController::HandleDeathMenuRespawnRequested);
		DeathMenuWidget->OnExitRequested.AddDynamic(this, &ACustomPlayerController::HandleDeathMenuExitRequested);
	}

	if (!DeathMenuWidget->IsInViewport())
	{
		DeathMenuWidget->AddToViewport(100);
	}

	DeathMenuWidget->SetVisibility(ESlateVisibility::Visible);

	SetShowMouseCursor(true);
	FInputModeUIOnly InputMode;
	SetInputMode(InputMode);
	SetIgnoreMoveInput(true);
	SetIgnoreLookInput(true);
}

void ACustomPlayerController::HideDeathMenu()
{
	if (!IsLocalController())
	{
		return;
	}

	if (DeathMenuWidget && DeathMenuWidget->IsInViewport())
	{
		DeathMenuWidget->RemoveFromParent();
	}

	SetShowMouseCursor(false);
	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);
	SetIgnoreMoveInput(false);
	SetIgnoreLookInput(false);
}

void ACustomPlayerController::HandleDeathMenuRespawnRequested()
{
	Server_RequestRespawn();
}

void ACustomPlayerController::HandleDeathMenuExitRequested()
{
	UKismetSystemLibrary::QuitGame(this, this, EQuitPreference::Quit, false);

	//ConsoleCommand(TEXT("disconnect"));
}

void ACustomPlayerController::SetDeathMenuVisible(bool bVisible)
{
	if (bVisible)
	{
		ShowDeathMenu();
		return;
	}

	HideDeathMenu();
}



///////////////////////////////////////////////////////////////////////////////
void ACustomPlayerController::Server_RequestWorldServerList_Implementation()
{
	AHubGameMode* HubGameMode = GetWorld() ? GetWorld()->GetAuthGameMode<AHubGameMode>() : nullptr;
	if (!HubGameMode)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - HubGameMode is not found", FString(__FUNCTION__));
		return;
	}
	const TArray<FWorldServerView> Servers = HubGameMode->BuildWorldServersSnapshot();
	//UE_LOGFMT(LogProjectA, Log, "{0} - Sending server list to client, Count={1}", FString(__FUNCTION__), Servers.Num());
	Client_ReceiveWorldServerList(HubGameMode->BuildWorldServersSnapshot());
}

void ACustomPlayerController::Client_ReceiveWorldServerList_Implementation(const TArray<FWorldServerView>& Servers)
{
	if (!IsLocalController())
	{
		return;
	}

	PushWorldServerListToHud(Servers);
}

void ACustomPlayerController::PushWorldServerListToHud(const TArray<FWorldServerView>& Servers)
{
	ABaseHud* BaseHud = Cast<ABaseHud>(GetHUD());
	if (!BaseHud)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - BaseHud is not found", FString(__FUNCTION__));
		return;
	}

	BaseHud->UpdateServerSelectionTable(Servers);
	UE_LOGFMT(LogProjectA, Log, "{0} - Push to HUD, Count={1}", FString(__FUNCTION__), Servers.Num());
}

void ACustomPlayerController::Server_RequestCharacterList_Implementation(FName ServerId)
{
	AHubGameMode* HubGameMode = GetWorld() ? GetWorld()->GetAuthGameMode<AHubGameMode>() : nullptr;
	if (!HubGameMode || ServerId.IsNone())
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - HubGameMode is not found or ServerId is None", FString(__FUNCTION__));
		return;
	}

	const TArray<FCharacterSelectionView> Characters = HubGameMode->BuildMockCharactersForServer(ServerId);

	if(Characters.Num() == 0)
	{
		UE_LOGFMT(LogProjectA, Log, "{0} - No characters found for ServerId={1}", FString(__FUNCTION__), ServerId.ToString());
	}

	Client_ReceiveCharacterList(/*ServerId,*/ Characters);
}

void ACustomPlayerController::Client_ReceiveCharacterList_Implementation(/*FName ServerId,*/ const TArray<FCharacterSelectionView>& Characters)
{
	if (IsRunningDedicatedServer())
	{
		return;
	}

	/*PendingSelectedServerId = ServerId;*/
	PushCharacterListToHud(Characters);
}

void ACustomPlayerController::PushCharacterListToHud(const TArray<FCharacterSelectionView>& Characters)
{
	ABaseHud* BaseHud = Cast<ABaseHud>(GetHUD());
	if (!BaseHud)
	{
		return;
	}

	CachedCharacterEntries.Reset();
	for (const FCharacterSelectionView& Item : Characters)
	{
		UCharacterEntryObject* EntryObject = NewObject<UCharacterEntryObject>(this);
		if (!EntryObject)
		{
			continue;
		}

		EntryObject->Data = Item;
		CachedCharacterEntries.Add(EntryObject);
	}

	TArray<UCharacterEntryObject*> Entries{};
	for (const TObjectPtr<UCharacterEntryObject>& Item : CachedCharacterEntries)
	{
		Entries.Add(Item.Get());
	}

	BaseHud->ShowCharacterSelectionScreen();
	BaseHud->UpdateCharacterSelectionTable(Entries);
}

void ACustomPlayerController::Server_RequestTravelToWorldWithCharacter_Implementation(FName ServerId, FName CharacterId)
{
	AHubGameMode* HubGameMode = GetWorld() ? GetWorld()->GetAuthGameMode<AHubGameMode>() : nullptr;
	if (!HubGameMode || ServerId.IsNone() || CharacterId.IsNone())
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - HubGameMode is not found or ServerId/CharacterId is None", FString(__FUNCTION__));
		return;
	}

	HubGameMode->EnterToWorldWithCharacter(this, ServerId, CharacterId);
}


///////////////////////////////////////////////////////////////////////////////
//AutoTests

#if WITH_DEV_AUTOMATION_TESTS

const TArray<FInputMappingContextWithPriority>& ACustomPlayerController::AutoTestGetInputMappingContext() const
{    
	return InputMappingContext;
}

#endif //WITH_DEV_AUTOMATION_TESTS




//void ACustomPlayerController::Server_RequestTravelToWorldServer_Implementation(FName ServerId)
//{
//	if (ServerId.IsNone())
//	{
//		UE_LOGFMT(LogProjectA, Warning, "{0} - ServerId is None", FString(__FUNCTION__));
//		return;
//	}
//
//	AHubGameMode* HubGameMode = GetWorld() ? GetWorld()->GetAuthGameMode<AHubGameMode>() : nullptr;
//	if (!HubGameMode)
//	{
//		UE_LOGFMT(LogProjectA, Warning, "{0} - HubGameMode is not found", FString(__FUNCTION__));
//		return;
//	}
//
//	HubGameMode->EnterToWorld(this, ServerId);
//}
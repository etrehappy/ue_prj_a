#include "Core/GameInstanceBase.h"

#include "Kismet/GameplayStatics.h"
#include "Engine/AssetManager.h"
#include "ProjectNetworkSettings.h"
#include "BaseHud.h"

#include "ProjectALog.h"

void UGameInstanceBase::Init()
{
	Super::Init();     
	UE_LOGFMT(LogProjectA, Log,
		"\n"
		"\n##################################################################################\n"
		"# {0} — GameInstance Init\n"
		"##################################################################################\n",
		FString(__FUNCTION__));
	 
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject( this,
		&UGameInstanceBase::OnPostLoadMap
	);


	if (GEngine)
	{
		GEngine->OnNetworkFailure().AddUObject(this, &UGameInstanceBase::HandleEngineNetworkFailure);
		GEngine->OnTravelFailure().AddUObject(this, &UGameInstanceBase::HandleEngineTravelFailure);
	}
}

void UGameInstanceBase::Shutdown()
{        
	UE_LOGFMT(LogProjectA, Log,
		"\n"
		"\n##################################################################################\n"
		"# {0} — GameInstance Shutdown\n"
		"##################################################################################\n",
		FString(__FUNCTION__));

	if (GEngine)
	{
		GEngine->OnNetworkFailure().RemoveAll(this);
		GEngine->OnTravelFailure().RemoveAll(this);
	}

	Super::Shutdown();
}

void UGameInstanceBase::OnStart()
{
	Super::OnStart();

	if (IsRunningDedicatedServer())
	{ 
		UE_LOGFMT(LogProjectA, Log, "{0} - Running Dedicated Server, skipping this", FString(__FUNCTION__));
		return;
	}
}

void UGameInstanceBase::OnPostLoadMap(UWorld* LoadedWorld)
{
	UE_LOGFMT(LogProjectA, Log, "{0} - Map loaded: {1}", FString(__FUNCTION__), *LoadedWorld->GetName() );

	PreloadInventoryItemDefinitions();
	
	if (IsRunningDedicatedServer())
	{
		UE_LOGFMT(LogProjectA, Log, "{0} - Running Dedicated Server, skipping this", FString(__FUNCTION__));
		return;
	}

	ABaseHud* BaseHud = Cast<ABaseHud>(GetWorld()->GetFirstPlayerController()->GetHUD());
	if (!BaseHud)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - BaseHud is not found", FString(__FUNCTION__));
		return;
	}

	if (!PendingErrorMessage.IsEmpty())
	{		
		BaseHud->ShowErrorScreen(PendingErrorMessage);		
		PendingErrorMessage.Empty();
		return;
	}

	BaseHud->HideNetConnectionStatusWidget();
}


void UGameInstanceBase::ExitToDesktop()
{
	//UE_LOGFMT(LogProjectA, Log, "{0} - Exiting to desktop...", FString(__FUNCTION__));
	const TCHAR* CallSite = TEXT("__FUNCTION__");
	FGenericPlatformMisc::RequestExit(false, CallSite);
}

void UGameInstanceBase::SaveNetStatusWidgetRef(UUserWidget* Widget)
{
	if (!Widget)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Widget is null", FString(__FUNCTION__));
		return;
	}

	NetConnectionStatusWidget = Widget;
}

void UGameInstanceBase::HandleEngineNetworkFailure(UWorld* /*World*/, UNetDriver* /*NetDriver*/, ENetworkFailure::Type FailureType, const FString& ErrorString)
{
	UE_LOGFMT(LogProjectA, Error, "{0} - Network error occurred: {1}", FString(__FUNCTION__), ErrorString);
	
	if (IsRunningDedicatedServer())
	{
		//UE_LOGFMT(LogProjectA, Log, "{0} - Running Dedicated Server, skipping this", FString(__FUNCTION__));
		return;
	}

	FString ErrorMessage = ErrorString;
	PendingErrorMessage = ErrorMessage;
}

void UGameInstanceBase::HandleEngineTravelFailure(UWorld* /*World*/, ETravelFailure::Type FailureType, const FString& ErrorString)
{
	UE_LOGFMT(LogProjectA, Error, "{0} - Travel error occurred: {1}", FString(__FUNCTION__), ErrorString);
	
	if (IsRunningDedicatedServer())
	{
		//UE_LOGFMT(LogProjectA, Log, "{0} - Running Dedicated Server, skipping this", FString(__FUNCTION__));
		return;
	}

	FString ErrorMessage = ErrorString;
	PendingErrorMessage = ErrorMessage;
}




///////////////////////////////////////////////////////////////////////////////
void UGameInstanceBase::PreloadInventoryItemDefinitions()
{
	UAssetManager& AssetManager = UAssetManager::Get();

	TArray<FPrimaryAssetId> AssetIds;
	AssetManager.GetPrimaryAssetIdList(FPrimaryAssetType(TEXT("InventoryItemDefinition")), AssetIds);

	if (AssetIds.Num() == 0)
	{
		UE_LOGFMT(LogProjectA, Log, "{0} - No InventoryItemDefinition primary assets found to preload", FString(__FUNCTION__));
		return;
	}

	//UE_LOGFMT(LogProjectA, Log, "{0} - Requesting preload of {1} InventoryItemDefinition assets", FString(__FUNCTION__), AssetIds.Num());

	AssetManager.LoadPrimaryAssets(AssetIds, TArray<FName>(), FStreamableDelegate::CreateUObject(this, &UGameInstanceBase::OnInventoryItemDefinitionsLoaded));

}

void UGameInstanceBase::OnInventoryItemDefinitionsLoaded()
{
	UE_LOGFMT(LogProjectA, Log, "{0} - InventoryItemDefinition primary assets loaded", FString(__FUNCTION__));
}

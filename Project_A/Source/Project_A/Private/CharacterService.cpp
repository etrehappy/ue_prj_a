
#include "CharacterService.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "Misc/ConfigCacheIni.h"
#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"
#include "ProjectALog.h"

void UCharacterService::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	GConfig->GetString(TEXT("/Script/Project_A.CharacterService"), TEXT("CharacterServerBaseUrl"),
		CharacterServerBaseUrl, GGameIni);

	//UE_LOGFMT(LogProjectA, Log, "{0} initialized. Server={1}", FString(__FUNCTION__), *CharacterServerBaseUrl);
}

void UCharacterService::Deinitialize()
{
	Super::Deinitialize();
}

void UCharacterService::FetchCharacterList(const FString& AccountId, const FString& SessionToken, FOnCharacterListReceived OnSuccess, FOnCharacterServiceError OnError)
{
	const FString Url = BuildUrl(FString::Printf(TEXT("/api/characters?accountId=%s"), *AccountId));

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = CreateAuthorizedRequest(SessionToken);
	Request->SetURL(Url);
	Request->SetVerb(TEXT("GET"));
	Request->OnProcessRequestComplete().BindUObject(this, &UCharacterService::OnCharacterListResponseReceived, OnSuccess, OnError);
	Request->ProcessRequest();
}

void UCharacterService::LoadCharacter(const FString& AccountId, const FName& CharacterId, const FString& SessionToken, FOnCharacterLoaded OnSuccess, FOnCharacterServiceError OnError)
{
	const FString Url = BuildUrl(FString::Printf(TEXT("/api/characters/%s?accountId=%s"), *CharacterId.ToString(), *AccountId));

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = CreateAuthorizedRequest(SessionToken);
	Request->SetURL(Url);
	Request->SetVerb(TEXT("GET"));
	Request->OnProcessRequestComplete().BindUObject(this, &UCharacterService::OnCharacterLoadResponseReceived, OnSuccess, OnError);
	Request->ProcessRequest();
}

void UCharacterService::SaveCharacter(const FString& AccountId, const FString& SessionToken, const FCharacterSaveData& SaveData, FOnCharacterSaved OnSuccess, FOnCharacterServiceError OnError)
{
	const FString Url = BuildUrl(FString::Printf(TEXT("/api/characters/%s"), *SaveData.CharacterId.ToString()));

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = CreateAuthorizedRequest(SessionToken);
	Request->SetURL(Url);
	Request->SetVerb(TEXT("POST"));
	Request->SetContentAsString(SerializeSaveData(AccountId, SaveData));
	Request->OnProcessRequestComplete().BindUObject(this, &UCharacterService::OnCharacterSaveResponseReceived, OnSuccess, OnError);
	Request->ProcessRequest();
}

void UCharacterService::OnCharacterListResponseReceived(FHttpRequestPtr /*Request*/, FHttpResponsePtr Response, bool bConnected, FOnCharacterListReceived OnSuccess, FOnCharacterServiceError OnError)
{
	// 1. Validation
	if (!bConnected || !Response.IsValid())
	{
		OnError.ExecuteIfBound(TEXT("No response from character server."));
		return;
	}

	if (Response->GetResponseCode() != 200)
	{
		OnError.ExecuteIfBound(FString::Printf(TEXT("Character list error (HTTP %d)"), Response->GetResponseCode()));
		return;
	}

	// 2. Get JSON array from response

	TArray<TSharedPtr<FJsonValue>> JsonArray;
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
	if (!FJsonSerializer::Deserialize(Reader, JsonArray))
	{
		OnError.ExecuteIfBound(TEXT("Failed to parse character list response."));
		return;
	}

	// 3. Convert JSON array to TArray<FCharacterSelectionView>

	TArray<FCharacterSelectionView> Characters;
	for (const TSharedPtr<FJsonValue>& Entry : JsonArray)
	{
		const TSharedPtr<FJsonObject> Obj = Entry->AsObject();
		if (!Obj.IsValid()) { continue; }

		FCharacterSelectionView View;
		View.CharacterId = FName(*Obj->GetStringField(TEXT("characterId")));
		View.CharacterName = Obj->GetStringField(TEXT("name"));
		View.Level = static_cast<int32>(Obj->GetNumberField(TEXT("level")));
		View.ClassName = FName(*Obj->GetStringField(TEXT("className")));
		Characters.Add(View);
	}

	// 4. Success callback

	OnSuccess.ExecuteIfBound(Characters);
}

void UCharacterService::OnCharacterLoadResponseReceived(FHttpRequestPtr /*Request*/, FHttpResponsePtr Response, bool bConnected, FOnCharacterLoaded OnSuccess, FOnCharacterServiceError OnError)
{
	// 1) Validate response and parse JSON
	TSharedPtr<FJsonObject> Json;
	FString ErrorMsg;
	if (!TryValidateAndParseJson(Response, bConnected, Json, ErrorMsg))
	{
		OnError.ExecuteIfBound(ErrorMsg);
		return;
	}

	// 2) Convert JSON -> FCharacterSaveData
	FCharacterSaveData SaveData;
	PopulateSaveDataFromJson(Json, SaveData);

	// 3) success callback
	OnSuccess.ExecuteIfBound(SaveData);
}

bool UCharacterService::TryValidateAndParseJson(FHttpResponsePtr Response, bool bConnected, TSharedPtr<FJsonObject>& OutJson, FString& OutError) const
{
	if (!bConnected || !Response.IsValid())
	{
		OutError = TEXT("No response from character server.");
		return false;
	}
		
	if (Response->GetResponseCode() != 200)
	{
		OutError = FString::Printf(TEXT("Character load error (HTTP %d)"), Response->GetResponseCode());
		return false;
	}

	// Parse JSON body
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
	if (!FJsonSerializer::Deserialize(Reader, OutJson) || !OutJson.IsValid())
	{
		OutError = TEXT("Failed to parse character save data.");
		return false;
	}

	return true;
}

void UCharacterService::PopulateSaveDataFromJson(const TSharedPtr<FJsonObject>& Json, FCharacterSaveData& OutSaveData) const
{
	OutSaveData.CharacterId = FName(*Json->GetStringField(TEXT("characterId")));
	OutSaveData.CharacterName = Json->GetStringField(TEXT("name"));
	OutSaveData.Level = static_cast<int32>(Json->GetNumberField(TEXT("level")));
	OutSaveData.ClassName = Json->GetStringField(TEXT("className"));
	OutSaveData.Experience = static_cast<int32>(Json->GetNumberField(TEXT("experience")));
	OutSaveData.CurrentHealth = static_cast<float>(Json->GetNumberField(TEXT("currentHealth")));
	OutSaveData.MaxHealth = static_cast<float>(Json->GetNumberField(TEXT("maxHealth")));
	OutSaveData.LastMapId = FName(*Json->GetStringField(TEXT("lastMapId")));

	// helpers
	PopulatePositionFromJson(Json, OutSaveData);
}

void UCharacterService::PopulatePositionFromJson(const TSharedPtr<FJsonObject>& Json, FCharacterSaveData& OutSaveData) const
{
	const TSharedPtr<FJsonObject>* PosObj = nullptr;
	if (Json->TryGetObjectField(TEXT("lastPosition"), PosObj) && PosObj != nullptr && (*PosObj).IsValid())
	{
		OutSaveData.LastPosition.X = static_cast<float>((*PosObj)->GetNumberField(TEXT("x")));
		OutSaveData.LastPosition.Y = static_cast<float>((*PosObj)->GetNumberField(TEXT("y")));
		OutSaveData.LastPosition.Z = static_cast<float>((*PosObj)->GetNumberField(TEXT("z")));
	}
}

void UCharacterService::OnCharacterSaveResponseReceived(FHttpRequestPtr /*Request*/, FHttpResponsePtr Response, bool bConnected, FOnCharacterSaved OnSuccess, FOnCharacterServiceError OnError)
{
	if (!bConnected || !Response.IsValid())
	{
		OnError.ExecuteIfBound(TEXT("No response from character server during save."));
		return;
	}

	const int32 Code = Response->GetResponseCode();
	if (Code != 200 && Code != 204)
	{
		OnError.ExecuteIfBound(FString::Printf(TEXT("Character save error (HTTP %d)"), Code));
		return;
	}

	//UE_LOGFMT(LogProjectA, Log, "{0}: Character saved successfully.", FString(__FUNCTION__));
	OnSuccess.ExecuteIfBound();
}

FString UCharacterService::SerializeSaveData(const FString& AccountId, const FCharacterSaveData& SaveData) const
{
	// 1. Create JSON object
	TSharedPtr<FJsonObject> Root = MakeShared<FJsonObject>();

	// 2. Fields
	Root->SetStringField(TEXT("accountId"), AccountId);
	Root->SetStringField(TEXT("characterId"), SaveData.CharacterId.ToString());
	Root->SetStringField(TEXT("name"), SaveData.CharacterName);
	Root->SetNumberField(TEXT("level"), SaveData.Level);
	Root->SetNumberField(TEXT("experience"), SaveData.Experience);
	Root->SetNumberField(TEXT("currentHealth"), SaveData.CurrentHealth);
	Root->SetNumberField(TEXT("maxHealth"), SaveData.MaxHealth);
	Root->SetStringField(TEXT("lastMapId"), SaveData.LastMapId.ToString());

	TSharedPtr<FJsonObject> PosObj = MakeShared<FJsonObject>();
	PosObj->SetNumberField(TEXT("x"), SaveData.LastPosition.X);
	PosObj->SetNumberField(TEXT("y"), SaveData.LastPosition.Y);
	PosObj->SetNumberField(TEXT("z"), SaveData.LastPosition.Z);
	Root->SetObjectField(TEXT("lastPosition"), PosObj);
	
	// 3. Serialize JSON to string
	FString Output;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Output);
	FJsonSerializer::Serialize(Root.ToSharedRef(), Writer);

	return Output;
}

FString UCharacterService::BuildUrl(const FString& Endpoint) const
{
	return CharacterServerBaseUrl + Endpoint;
}

TSharedRef<IHttpRequest, ESPMode::ThreadSafe> UCharacterService::CreateAuthorizedRequest(const FString& SessionToken) const
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	Request->SetHeader(TEXT("Authorization"), FString::Printf(TEXT("Bearer %s"), *SessionToken));
	Request->SetTimeout(HttpTimeoutSeconds);
	return Request;
}
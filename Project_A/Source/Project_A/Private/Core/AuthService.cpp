
#include "Core/AuthService.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "Misc/ConfigCacheIni.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

#include "ProjectALog.h"

void UAuthService::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	GConfig->GetString(TEXT("/Script/Project_A.AuthService"), TEXT("AuthServerBaseUrl"),
		AuthServerBaseUrl, GGameIni);

	GConfig->GetBool(TEXT("/Script/Project_A.AuthService"), TEXT("bMockMode"),
		bMockMode, GGameIni);
}

void UAuthService::Deinitialize()
{
	Super::Deinitialize();
}

void UAuthService::Login(const FString& Username, const FString& Password,	FOnLoginSuccess OnSuccess, FOnLoginFailure OnFailure)
{
	/*if (bMockMode)
	{
		LoginMock(Username, Password, OnSuccess, OnFailure);
		return;
	}*/

	const TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(BuildUrl(TEXT("/api/auth/login")));
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	Request->SetTimeout(HttpTimeoutSeconds);

	// Build { "username": "...", "password": "..." }
	const FString Body = FString::Printf(TEXT("{\"username\":\"%s\",\"password\":\"%s\"}"),
		*Username, *Password);
	Request->SetContentAsString(Body);
	Request->OnProcessRequestComplete().BindUObject(this, &UAuthService::OnLoginResponseReceived, OnSuccess, OnFailure);
	Request->ProcessRequest();
}

void UAuthService::Logout(const FString& AccountId, const FString& SessionToken, FOnLogoutSuccess OnSuccess)
{
	/*if (bMockMode)
	{		
		UE_LOGFMT(LogProjectA, Log, "{0} logout successful [Mock Mode]", FString(__FUNCTION__));
		OnSuccess.ExecuteIfBound();
		return;
	}*/

	const TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(BuildUrl(TEXT("/api/auth/logout")));
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	Request->SetHeader(TEXT("Authorization"), FString::Printf(TEXT("Bearer %s"), *SessionToken));
	Request->SetTimeout(HttpTimeoutSeconds);

	const FString Body = FString::Printf(TEXT("{\"accountId\":\"%s\"}"), *AccountId);
	Request->SetContentAsString(Body);

	// Fire and forget — just log the result
	auto ResponseHandler = [OnSuccess](FHttpRequestPtr /*Req*/, FHttpResponsePtr Resp, bool bConnected)
		{
			if (bConnected && Resp.IsValid())
			{
				UE_LOGFMT(LogProjectA, Log, "{0} logout response: {1}", FString(__FUNCTION__), Resp->GetResponseCode());
			}
			OnSuccess.ExecuteIfBound();
		};

	Request->OnProcessRequestComplete().BindLambda(ResponseHandler);
	Request->ProcessRequest();
}

void UAuthService::ValidateSession(const FString& AccountId, const FString& SessionToken, FOnLoginSuccess OnSuccess, FOnLoginFailure OnFailure)
{
	//if (bMockMode)
	//{
	//	// In mock mode any non-empty token is considered valid
	//	if (!SessionToken.IsEmpty())
	//	{
	//		OnSuccess.ExecuteIfBound(AccountId, SessionToken);
	//	}
	//	else
	//	{
	//		UE_LOGFMT(LogProjectA, Log, "{0} session validation failed [Mock Mode]", FString(__FUNCTION__));
	//		OnFailure.ExecuteIfBound(TEXT("Invalid session token"));
	//	}
	//	return;
	//}

	const TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(BuildUrl(TEXT("/api/auth/validate")));
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	Request->SetHeader(TEXT("Authorization"), FString::Printf(TEXT("Bearer %s"), *SessionToken));
	Request->SetTimeout(HttpTimeoutSeconds);

	const FString Body = FString::Printf(TEXT("{\"accountId\":\"%s\"}"), *AccountId);
	Request->SetContentAsString(Body);
	Request->OnProcessRequestComplete().BindUObject(this, &UAuthService::OnLoginResponseReceived, OnSuccess, OnFailure);
	Request->ProcessRequest();
}

void UAuthService::OnLoginResponseReceived(FHttpRequestPtr /*Request*/, FHttpResponsePtr Response,	bool bSuccess, FOnLoginSuccess OnSuccess, FOnLoginFailure OnFailure)
{
	// 1. Validation
	if (!bSuccess || !Response.IsValid())
	{
		OnFailure.ExecuteIfBound(TEXT("No response from auth server. Check your connection."));
		return;
	}

	const int32 Code = Response->GetResponseCode();
	const FString Body = Response->GetContentAsString();

	
	if (Code != 200) // Get error message 
	{
		UE_LOGFMT(LogProjectA, Warning, "{0}: HTTP {1}, URL response body: {2}", FString(__FUNCTION__), Code, *Body);

		TSharedPtr<FJsonObject> Json;
		const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Body);
		FString ErrorMsg = FString::Printf(TEXT("Auth server error (HTTP %d)"), Code);
		if (FJsonSerializer::Deserialize(Reader, Json) && Json.IsValid())
		{
			FString Msg;
			if (Json->TryGetStringField(TEXT("message"), Msg))
			{
				ErrorMsg = Msg;
			}
		}
		OnFailure.ExecuteIfBound(ErrorMsg);
		return;
	}

	// 2. Parse the body and validate required fields before invoking success callback.

	TSharedPtr<FJsonObject> Json;
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Body);
	if (!FJsonSerializer::Deserialize(Reader, Json) || !Json.IsValid())
	{
		OnFailure.ExecuteIfBound(TEXT("Failed to parse auth server response."));
		return;
	}


	// 3. Extract required fields. If any are missing, treat it as an invalid response.

	FString AccountId, SessionToken;
	if (!Json->TryGetStringField(TEXT("accountId"), AccountId) ||
		!Json->TryGetStringField(TEXT("sessionToken"), SessionToken))
	{
		OnFailure.ExecuteIfBound(TEXT("Auth response missing required fields."));
		return;
	}

	OnSuccess.ExecuteIfBound(AccountId, SessionToken);
}

void UAuthService::LoginMock(const FString& Username, const FString& Password,
	FOnLoginSuccess OnSuccess, FOnLoginFailure OnFailure)
{	
	UE_LOGFMT(LogProjectA, Log, "{0} - Mock login attempt: Username={1}", FString(__FUNCTION__), *Username);

	// Reject empty credentials
	if (Username.IsEmpty() || Password.IsEmpty())
	{
		OnFailure.ExecuteIfBound(TEXT("Username and password must not be empty."));
		return;
	}

	// Mock: generate a deterministic AccountId and a fake token
	const FString AccountId = FString::Printf(TEXT("account_%s"), *Username.ToLower());
	const FString SessionToken = FString::Printf(TEXT("mock_token_%s_%d"),
		*Username.ToLower(), FMath::Rand());

	UE_LOGFMT(LogProjectA, Log, "{0} - Mock login successful: AccountId={1}, SessionToken={2}",
		FString(__FUNCTION__), *AccountId, *SessionToken);
	OnSuccess.ExecuteIfBound(AccountId, SessionToken);
}

FString UAuthService::BuildUrl(const FString& Endpoint) const
{
	return AuthServerBaseUrl + Endpoint;
}
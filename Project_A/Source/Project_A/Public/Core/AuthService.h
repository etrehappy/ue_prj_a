/*****************************************************************//**
 * \file   AuthService.h
 * \brief 
 *
 * \date   May 2026
 *********************************************************************/
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/IHttpRequest.h"

#include "AuthService.generated.h"

DECLARE_DELEGATE_TwoParams(FOnLoginSuccess, const FString& /*AccountId*/, const FString& /*SessionToken*/);
DECLARE_DELEGATE_OneParam(FOnLoginFailure, const FString& /*ErrorMessage*/);

DECLARE_DELEGATE(FOnLogoutSuccess);

/**
 * @class UAuthService
 * @brief Subsystem for player authentication.
 */
UCLASS()
class PROJECT_A_API UAuthService : public UGameInstanceSubsystem
{
	GENERATED_BODY()

						/* === C++ member functions === */
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	/**
	 * @brief Sends a login request to the auth server.
	 */
	void Login(const FString& Username, const FString& Password, FOnLoginSuccess OnSuccess, FOnLoginFailure OnFailure);

	/**
	 * @brief Sends a logout request to the auth server.
	 */
	void Logout(const FString& AccountId, const FString& SessionToken, FOnLogoutSuccess OnSuccess);

	/**
	 * @brief Validates a session token on the auth server (that a connecting client is authenticated).
	 */
	void ValidateSession(const FString& AccountId, const FString& SessionToken, FOnLoginSuccess OnSuccess, FOnLoginFailure OnFailure);

private:
	void OnLoginResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccess, FOnLoginSuccess OnSuccess, FOnLoginFailure OnFailure);

	void LoginMock(const FString& Username, const FString& Password, FOnLoginSuccess OnSuccess, FOnLoginFailure OnFailure);

	FString BuildUrl(const FString& Endpoint) const;


						/* === C++ member variables === */
private:
	/**
	 * @brief Override via DefaultGame.ini: [/Script/Project_A.AuthService]
	 */
	FString AuthServerBaseUrl{};

	/**
	 * @brief Override via DefaultGame.ini: [/Script/Project_A.AuthService] 
	 */
	bool bMockMode{false};

	float HttpTimeoutSeconds{10.f};
};

/*****************************************************************//**
 * \file   CharacterService.h
 * \brief  
 *
 * \date   May 2026
 *********************************************************************/
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/IHttpRequest.h"
#include "Character/CharacterSaveData.h"
#include "CharacterSelectionView.h"

#include "CharacterService.generated.h"

DECLARE_DELEGATE_OneParam(FOnCharacterListReceived, const TArray<FCharacterSelectionView>& /*Characters*/);
DECLARE_DELEGATE_OneParam(FOnCharacterLoaded, const FCharacterSaveData& /*SaveData*/);
DECLARE_DELEGATE_OneParam(FOnCharacterServiceError, const FString& /*ErrorMessage*/);
DECLARE_DELEGATE(FOnCharacterSaved);

/**
 * @class UCharacterService
 * @brief subsystem responsible for character persistence.
 */
UCLASS()
class PROJECT_A_API UCharacterService : public UGameInstanceSubsystem
{
	GENERATED_BODY()

						/* === C++ member functions === */
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	/**
	 * @brief Fetches the list of characters belonging to the account.
	 */
	void FetchCharacterList(const FString& AccountId, const FString& SessionToken, FOnCharacterListReceived OnSuccess, FOnCharacterServiceError OnError);

	/**
	 * @brief Loads full save data for a single character.
	 */
	void LoadCharacter(const FString& AccountId, const FName& CharacterId, const FString& SessionToken, FOnCharacterLoaded OnSuccess, FOnCharacterServiceError OnError);

	/**
	 * @brief Saves full character state to the character server.
	 */
	void SaveCharacter(const FString& AccountId, const FString& SessionToken, const FCharacterSaveData& SaveData, FOnCharacterSaved OnSuccess, FOnCharacterServiceError OnError);

private:
	void OnCharacterListResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnected, FOnCharacterListReceived OnSuccess, FOnCharacterServiceError OnError);
	void OnCharacterLoadResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnected, FOnCharacterLoaded OnSuccess, FOnCharacterServiceError OnError);
	void OnCharacterSaveResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnected, FOnCharacterSaved OnSuccess, FOnCharacterServiceError OnError);

	FString SerializeSaveData(const FString& AccountId, const FCharacterSaveData& SaveData) const;
	FString BuildUrl(const FString& Endpoint) const;

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> CreateAuthorizedRequest(const FString& SessionToken) const;
		
private:
	// Helpers. see OnCharacterLoadResponseReceived()
	bool TryValidateAndParseJson(FHttpResponsePtr Response, bool bConnected, TSharedPtr<FJsonObject>& OutJson, FString& OutError) const;
	void PopulateSaveDataFromJson(const TSharedPtr<FJsonObject>& Json, FCharacterSaveData& OutSaveData) const;
	void PopulatePositionFromJson(const TSharedPtr<FJsonObject>& Json, FCharacterSaveData& OutSaveData) const;


						/* === C++ member variables === */
private:
	/**
	 * @brief Override via DefaultGame.ini
	 */
	FString CharacterServerBaseUrl{};

	float HttpTimeoutSeconds{10.f};
};
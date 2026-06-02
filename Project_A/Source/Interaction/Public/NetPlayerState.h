/*****************************************************************//**
 * \file   NetPlayerState.h
 * \brief  Custom PlayerState class for networked gameplay, including party management.
 * 
 * \date   May 2026
 *********************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "NetPlayerState.generated.h"

/**
 * @class ANetPlayerState
 * @brief
 * @see AWorldGameMode
 */
UCLASS()
class INTERACTION_API ANetPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void SetPartyId(FName InPartyId) { PartyId = InPartyId; }
	FName GetPartyId() const { return PartyId; }

	/**
	 * @brief Set by AWorldGameMode::InitNewPlayer from URL options.
	 * Used by server systems to identify the owning account.
	 */
	void SetAccountId(const FString& InAccountId) { AccountId = InAccountId; }
	FString GetAccountId() const { return AccountId; }

	/**
	 * @brief Set by AWorldGameMode::InitNewPlayer from URL options.
	 * Used by server systems (CharacterService save/load) to identify the active character.
	 */
	void SetCharacterId(FName InCharacterId) { CharacterId = InCharacterId; }
	FName GetCharacterId() const { return CharacterId; }

	/**
	 * @brief Session token forwarded from Hub. Used by server to call auth/character services.
	 * Never replicated to clients.
	 */
	void SetSessionToken(const FString& InToken) { SessionToken = InToken; }
	FString GetSessionToken() const { return SessionToken; }

	void SetCharacterName(const FString& InCharacterName) { CharacterName = InCharacterName; }
	FString GetCharacterName() const { return CharacterName; }

	void SetLevel(int32 InLevel) { Level = InLevel; }
	int32 GetLevel() const { return Level; }

	void SetClassName(const FString& InClassName) { ClassName = InClassName; }
	FString GetClassName() const { return ClassName; }

protected:
	UPROPERTY(Replicated, BlueprintReadOnly)
	FName PartyId{NAME_None};

	UPROPERTY(Replicated, BlueprintReadOnly)
	int32 Level{0};

	UPROPERTY(Replicated, BlueprintReadOnly)
	FString CharacterName{};

	UPROPERTY(Replicated, BlueprintReadOnly)
	FString ClassName{};

private:
	// Not replicated — server-only auth data
	FString AccountId{};
	FString SessionToken{};
	FName   CharacterId{NAME_None};
	
	
};

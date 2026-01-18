

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/DamageType.h"
#include "WspDamageType.generated.h"


UENUM(BlueprintType)
enum class EInjuryType : uint8
{
	None					UMETA(DisplayName = "None")
	, Physical				UMETA(DisplayName = "Physical")
	, Magic					UMETA(DisplayName = "Magic")
	, Heal					UMETA(DisplayName = "Heal")
};

UENUM(BlueprintType)
enum class EElementalInjuryType : uint8
{
	None					UMETA(DisplayName = "None")
	, Fire					UMETA(DisplayName = "Fire")
	, Ice					UMETA(DisplayName = "Ice")
};


UCLASS(Blueprintable)
class WEAPONSYSTEMPLUGIN_API UWspDamageType : public UDamageType
{
	GENERATED_BODY()

public:	
	UWspDamageType() = default;
	virtual ~UWspDamageType() = default;

	EInjuryType GetDamageType() const;
	EElementalInjuryType GetElementalDamageType() const;

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	EInjuryType DamageType{EInjuryType::None};

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	EElementalInjuryType ElementalType{EElementalInjuryType::None};
};

#include "StatusEffect/StatusEffectData.h"

FPrimaryAssetId UStatusEffectData::GetPrimaryAssetId() const
{
	return FPrimaryAssetId(TEXT("StatusEffect"), GetFName());
}


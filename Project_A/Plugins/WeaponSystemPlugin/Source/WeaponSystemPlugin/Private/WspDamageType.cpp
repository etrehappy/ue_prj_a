


#include "WspDamageType.h"

EInjuryType UWspDamageType::GetDamageType() const
{
	return DamageType;
}

EElementalInjuryType UWspDamageType::GetElementalDamageType() const
{
	return ElementalType;
}

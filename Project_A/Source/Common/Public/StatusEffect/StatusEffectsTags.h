/*****************************************************************//**
 * \file   StatusEffectsTegs.h
 * \brief  It is a simple way to compare stat tags.
 * \todo   It should be implemented in another way. For example, as a subsystem or manager that caches tags and provides them to other classes.
 *
 * \see		.\Project_A\Config\Tags\StatusEffectTags.ini
 * \see		.\Project_A\Config\Tags\StatTags.ini
 * \date   April 2026
 *********************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

namespace StatusEffectsTags
{
	FORCEINLINE const FGameplayTag& HealthTag()
	{
		static const FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("Stats.General.Health"));
		return Tag;
	}

	FORCEINLINE const FGameplayTag& DefenseTag()
	{
		static const FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("Stats.General.Defense"));
		return Tag;
	}
}
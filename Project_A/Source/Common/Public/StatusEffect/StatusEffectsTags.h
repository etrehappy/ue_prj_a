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
	static const FGameplayTag HealthTag = FGameplayTag::RequestGameplayTag(TEXT("Stats.General.Health"));
	static const FGameplayTag DefenseTag = FGameplayTag::RequestGameplayTag(TEXT("Stats.General.Defense"));
}

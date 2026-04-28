#include "NpcAttackLogicBase.h"

#include "NpcBattleComponent.h"

void UNpcAttackLogicBase::Initialize(UNpcBattleComponent* InBattleComponent)
{
	BattleComponent = InBattleComponent;
}
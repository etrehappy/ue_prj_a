#include "NpcAIController.h"

#include "NpcBase.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Components/StateTreeAIComponent.h"

#include "ProjectALog.h"

ANpcAIController::ANpcAIController()
{
	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComponent"));
	SetPerceptionComponent(*AIPerceptionComponent);

	StateTreeAIComponent = CreateDefaultSubobject<UStateTreeAIComponent>(TEXT("StateTreeAIComponent"));

	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	SightConfig->SightRadius = 1800.f;
	SightConfig->LoseSightRadius = 2200.f;
	SightConfig->PeripheralVisionAngleDegrees = 90.f;
	SightConfig->SetMaxAge(2.0f);
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;

	AIPerceptionComponent->ConfigureSense(*SightConfig);
	AIPerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());


}

void ANpcAIController::BeginPlay()
{
	Super::BeginPlay();

	if (AIPerceptionComponent)
	{
		AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &ANpcAIController::HandleTargetPerceptionUpdated);
	}

	if (!StateTreeAIComponent)
	{
		UE_LOGFMT(LogProjectA, Error, "{0} - StateTreeAIComponent is not set on {1}", FString(__FUNCTION__), *GetName());
	}
}

void ANpcAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
}

void ANpcAIController::HandleTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	ANpcBase* ControlledNpc = Cast<ANpcBase>(GetPawn());
	if (!ControlledNpc || !Actor || Actor == ControlledNpc)
	{
		return;
	}

	if (Stimulus.WasSuccessfullySensed() && IsHostileActor(Actor))
	{
		ControlledNpc->SetCurrentTarget(Actor);
		SetFocus(Actor);
		if (StateTreeAIComponent)
		{
			StateTreeAIComponent->RestartLogic();
		}
		return;
	}

	if (ControlledNpc->GetCurrentTarget() == Actor)
	{
		ClearCurrentTarget();
	}
}

bool ANpcAIController::IsHostileActor(const AActor* Actor) const
{
	const ANpcBase* ControlledNpc = Cast<ANpcBase>(GetPawn());
	if (!ControlledNpc || !Actor)
	{
		return false;
	}

	const ANpcBase* OtherNpc = Cast<ANpcBase>(Actor);
	if (OtherNpc)
	{
		return OtherNpc->GetNpcFaction() != ControlledNpc->GetNpcFaction();
	}

	return ControlledNpc->GetNpcFaction() == ENpcFaction::Enemy;
}

void ANpcAIController::ClearCurrentTarget()
{
	ANpcBase* ControlledNpc = Cast<ANpcBase>(GetPawn());
	if (!ControlledNpc)
	{
		return;
	}

	ControlledNpc->SetCurrentTarget(nullptr);
	ClearFocus(EAIFocusPriority::Gameplay);
	StopMovement();
	OnCurrentTargetCleared();
}
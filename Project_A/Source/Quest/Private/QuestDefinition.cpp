
#include "QuestDefinition.h"

bool UQuestDefinition::BuildInitialQuestInstance(FQuestInstance& OutQuestInstance) const
{
	if (QuestId.IsNone())
	{
		return false;
	}

	OutQuestInstance = FQuestInstance{};
	OutQuestInstance.QuestId = QuestId;
	OutQuestInstance.SharingMode = SharingMode;
	OutQuestInstance.Status = EQuestStatus::Accepted;
	OutQuestInstance.Objectives.Reserve(Objectives.Num());

	for (const FQuestObjectiveDefinition& ObjectiveDef : Objectives)
	{
		if (!ObjectiveDef.ObjectiveTag.IsValid())
		{
			continue;
		}

		FQuestObjectiveRuntime RuntimeObjective{};
		RuntimeObjective.ObjectiveTag = ObjectiveDef.ObjectiveTag;
		RuntimeObjective.CurrentValue = 0;
		RuntimeObjective.TargetValue = FMath::Max(1, ObjectiveDef.TargetValue);

		OutQuestInstance.Objectives.Add(RuntimeObjective);
	}

	return true;
}


UQuestTrackRowObject* UQuestTrackRowObject::CreateQuestTrackRow(const FText& InDescription, int32 InTargetValue, int32 InCurrentValue, bool bInShowValueInUI)
{
	UQuestTrackRowObject* NewObj = NewObject<UQuestTrackRowObject>();
	NewObj->ObjectiveDescription = InDescription;
	NewObj->TargetValue = InTargetValue;
	NewObj->CurrentValue = InCurrentValue;
	NewObj->bShowValueInUI = bInShowValueInUI;
	return NewObj;
}

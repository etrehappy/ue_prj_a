#include "QuestJournalWidget.h"

#include "QuestDefinition.h"
#include "QuestLogComponent.h"
#include "QuestManagerSubsystem.h"
#include "QuestTypes.h"
#include "QuestJournalEntryWidget.h"

#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"

void UQuestJournalWidget::InitialiseWithQuestLog(UQuestLogComponent* InQuestLogComponent)
{
	if (!InQuestLogComponent)
	{
		return;
	}

	if (QuestLogComponent)
	{
		QuestLogComponent->OnQuestLogUpdated.RemoveDynamic(this, &UQuestJournalWidget::HandleQuestLogUpdated);
	}

	QuestLogComponent = InQuestLogComponent;
	QuestLogComponent->OnQuestLogUpdated.AddDynamic(this, &UQuestJournalWidget::HandleQuestLogUpdated);
}


void UQuestJournalWidget::HandleQuestLogUpdated()
{	
	OnQuestLogUpdated();
}

void UQuestJournalWidget::RebuildEntries()
{
	if (!QuestListBox)
	{
		return;
	}

	QuestListBox->ClearChildren();
	EntryWidgets.Reset();

	if (!QuestLogComponent || !QuestJournalEntryWidgetClass)
	{
		if (EmptyStateText)
		{
			EmptyStateText->SetVisibility(ESlateVisibility::Visible);
		}
		return;
	}

	const TArray<FQuestInstance>& QuestInstances = QuestLogComponent->GetQuestInstances();
	if (QuestInstances.Num() == 0)
	{
		if (EmptyStateText)
		{
			EmptyStateText->SetVisibility(ESlateVisibility::Visible);
		}
		return;
	}

	if (EmptyStateText)
	{
		EmptyStateText->SetVisibility(ESlateVisibility::Collapsed);
	}

	const FName TrackedQuestId = QuestLogComponent->GetTrackedQuestId();

	for (const FQuestInstance& QuestInstance : QuestInstances)
	{
		UQuestJournalEntryWidget* EntryWidget = CreateWidget<UQuestJournalEntryWidget>(GetOwningPlayer(), QuestJournalEntryWidgetClass);
		if (!EntryWidget)
		{
			continue;
		}

		const FText QuestTitle = ResolveQuestTitle(QuestInstance.QuestId);
		const bool bIsTracked = (TrackedQuestId == QuestInstance.QuestId);

		EntryWidget->Setup(QuestInstance.QuestId, QuestTitle);	

		QuestListBox->AddChild(EntryWidget);
		EntryWidgets.Add(EntryWidget);
	}
}

FText UQuestJournalWidget::ResolveQuestTitle(FName QuestId) const
{
	if (QuestId.IsNone())
	{
		return FText::GetEmpty();
	}

	UWorld* World = GetWorld();
	UQuestManagerSubsystem* QuestManagerSubsystem = World ? World->GetSubsystem<UQuestManagerSubsystem>() : nullptr;
	const UQuestDefinition* QuestDefinition = QuestManagerSubsystem ? QuestManagerSubsystem->GetQuestDefinition(QuestId) : nullptr;

	if (QuestDefinition && !QuestDefinition->DisplayName.IsEmpty())
	{
		return QuestDefinition->DisplayName;
	}

	return FText::FromName(QuestId);
}

void UQuestJournalWidget::HandleTrackRequested(FName QuestId, bool bIsChecked)
{
	if (!QuestLogComponent)
	{
		return;
	}

	if (bIsChecked)
	{
		QuestLogComponent->SetTrackedQuestId(QuestId);
	}
	else if (QuestLogComponent->GetTrackedQuestId() == QuestId)
	{
		QuestLogComponent->SetTrackedQuestId(NAME_None);
	}
}
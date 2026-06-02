#include "QuestJournalEntryWidget.h"

#include "Components/CheckBox.h"
#include "Components/TextBlock.h"

void UQuestJournalEntryWidget::Setup(FName InQuestId, const FText& InQuestTitle)
{
	QuestId = InQuestId;

	if (QuestTitleText)
	{
		QuestTitleText->SetText(InQuestTitle);
	}
}

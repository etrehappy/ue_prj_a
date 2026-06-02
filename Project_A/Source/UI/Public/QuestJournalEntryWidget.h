#pragma once

#include "CoreMinimal.h"
#include "BaseWidget.h"
#include "QuestTypes.h"

#include "QuestJournalEntryWidget.generated.h"

class UCheckBox;
class UTextBlock;

UCLASS()
class UI_API UQuestJournalEntryWidget : public UBaseWidget
{
	GENERATED_BODY()

public:
	void Setup(FName InQuestId, const FText& InQuestTitle);


private:
	FName QuestId{NAME_None};
	bool bIgnoreCheckChanged{false};

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> QuestTitleText{};
};
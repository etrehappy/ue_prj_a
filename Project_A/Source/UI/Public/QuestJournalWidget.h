#pragma once

#include "CoreMinimal.h"
#include "BaseWidget.h"

#include "QuestJournalWidget.generated.h"

class UQuestLogComponent;
class UQuestJournalEntryWidget;
class UQuestDefinition;
class UVerticalBox;
class UTextBlock;

UCLASS()
class UI_API UQuestJournalWidget : public UBaseWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Quest|UI")
	void InitialiseWithQuestLog(UQuestLogComponent* InQuestLogComponent);

protected:
	UFUNCTION()
	void HandleQuestLogUpdated();

	UFUNCTION(BlueprintImplementableEvent, Category = "Quest|UI")
	void OnQuestLogUpdated();


private:
	void RebuildEntries();
	FText ResolveQuestTitle(FName QuestId) const;
	void HandleTrackRequested(FName QuestId, bool bIsChecked);

private:


	UPROPERTY()
	TArray<TObjectPtr<UQuestJournalEntryWidget>> EntryWidgets{};

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UVerticalBox> QuestListBox{};

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> EmptyStateText{};

	UPROPERTY(EditDefaultsOnly, Category = "Quest|UI")
	TSubclassOf<UQuestJournalEntryWidget> QuestJournalEntryWidgetClass{};


	UPROPERTY(BlueprintReadOnly, Category = "Quest|UI")
	TObjectPtr<UQuestLogComponent> QuestLogComponent{};
};
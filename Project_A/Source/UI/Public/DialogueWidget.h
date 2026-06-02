

#pragma once

#include "CoreMinimal.h"
#include "BaseWidget.h"
#include "DialogueManagerSubsystem.h"

#include "DialogueWidget.generated.h"

UCLASS()
class UI_API UDialogueWidget : public UBaseWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void ShowDialogueNode(const FDialogueNodeRuntime& Node);

	UFUNCTION(BlueprintCallable)
	void HideDialogue();

	UFUNCTION(BlueprintCallable)
	void SubmitChoice(FName ChoiceId);

	UFUNCTION(BlueprintCallable)
	void CloseDialogue();

	UFUNCTION(BlueprintPure)
	const FDialogueNodeRuntime& GetCurrentNode() const { return CurrentNode; }

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "Dialogue")
	void BP_OnDialogueNodeUpdated(const FDialogueNodeRuntime& Node);

	UFUNCTION(BlueprintImplementableEvent, Category = "Dialogue")
	void BP_OnDialogueVisibilityChanged(bool bVisible);

private:
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FDialogueNodeRuntime CurrentNode{};
};
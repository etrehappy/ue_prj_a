
/*****************************************************************//**
 * \file   LoginWidget.h
 * \brief  Login screen widget.
 *
 * \date   May 2026
 *********************************************************************/
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LoginWidget.generated.h"

class UEditableTextBox;
class UButton;
class UTextBlock;
class UWidget;

/**
 * @class ULoginWidget
 * @brief 
 *
 * @details Responsibilities:
 *  - Collect username and password from the player.
 *  - Call UAuthService::Login().
 *  - On success: save AccountId + SessionToken to UGameInstanceBase, then connect to Hub-server.
 *  - On failure: display an error message and re-enable the login button.
 */
UCLASS(Abstract)
class PROJECT_A_API ULoginWidget : public UUserWidget
{
	GENERATED_BODY()

						/* === C++ member functions === */
protected:
	virtual void NativeConstruct() override;

private:
	/**
	 * @brief Reads credentials from input fields and calls UAuthService::Login().
	 * Bound to LoginButton::OnClicked in NativeConstruct.
	 */
	UFUNCTION()
	void OnLoginButtonClicked();

	void OnLoginSuccess(const FString& AccountId, const FString& SessionToken);
	void OnLoginFailure(const FString& ErrorMessage);

	/**
	 * @brief Switches the widget into a "waiting for server" state:
	 * disables the login button and shows the loading overlay.
	 */
	void SetLoadingState(bool bLoading);

						/* === Unreal Engine UPROPERTY === */
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEditableTextBox> UsernameInput{};

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEditableTextBox> PasswordInput{};

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> LoginButton{};

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ErrorText{};

	/**
	 * @brief Overlay shown while the auth request is in progress.
	 */
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UWidget> LoadingOverlay{};

	/**
	 * @brief to play a transition animation, etc.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Login")
	void OnLoginAnimationSuccess();

	/**
	 * @brief to play a shake animation on the input fields, etc.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Login")
	void OnLoginAnimationFailure();
};
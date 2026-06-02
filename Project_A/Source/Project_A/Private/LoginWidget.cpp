
#include "LoginWidget.h"

#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "Components/Widget.h"
#include "Core/AuthService.h"
#include "Core/ClientStartGameMode.h"
#include "Core/GameInstanceBase.h"

#include "ProjectALog.h"

void ULoginWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Hide error and loading state on start
	if (ErrorText)
	{
		ErrorText->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (LoadingOverlay)
	{
		LoadingOverlay->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (LoginButton)
	{
		LoginButton->OnClicked.AddDynamic(this, &ULoginWidget::OnLoginButtonClicked);
	}
}

void ULoginWidget::OnLoginButtonClicked()
{
	if (!UsernameInput || !PasswordInput)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0}: UsernameInput or PasswordInput is not bound.", FString(__FUNCTION__));
		return;
	}

	const FString Username = UsernameInput->GetText().ToString().TrimStartAndEnd();
	const FString Password = PasswordInput->GetText().ToString();

	if (Username.IsEmpty() || Password.IsEmpty())
	{
		if (ErrorText)
		{
			ErrorText->SetText(FText::FromString(TEXT("Please enter your username and password.")));
			ErrorText->SetVisibility(ESlateVisibility::Visible);
		}
		return;
	}

	SetLoadingState(true);

	UAuthService* AuthService = GetGameInstance()->GetSubsystem<UAuthService>();
	if (!AuthService)
	{
		UE_LOGFMT(LogProjectA, Error, "{0}: UAuthService subsystem not found.", FString(__FUNCTION__));
		SetLoadingState(false);
		return;
	}

	AuthService->Login(
		Username,
		Password,
		FOnLoginSuccess::CreateUObject(this, &ULoginWidget::OnLoginSuccess),
		FOnLoginFailure::CreateUObject(this, &ULoginWidget::OnLoginFailure)
	);
}

void ULoginWidget::OnLoginSuccess(const FString& AccountId, const FString& SessionToken)
{
	UE_LOGFMT(LogProjectA, Log, "{0}: Login successful. AccountId={1}", FString(__FUNCTION__), *AccountId);

	UGameInstanceBase* GI = Cast<UGameInstanceBase>(GetGameInstance());
	if (!GI)
	{
		UE_LOGFMT(LogProjectA, Error, "{0}: GameInstance is not UGameInstanceBase.", FString(__FUNCTION__));
		SetLoadingState(false);
		return;
	}

	GI->SetAccountId(AccountId);
	GI->SetSessionToken(SessionToken);

	OnLoginAnimationSuccess();

	// Connect to Hub-server
	AClientStartGameMode* GM = Cast<AClientStartGameMode>(GetWorld()->GetAuthGameMode());
	if (GM)
	{
		GM->ConnectToHubServer();
	}
	else
	{
		UE_LOGFMT(LogProjectA, Error, "{0}: AClientStartGameMode not found.", FString(__FUNCTION__));
		SetLoadingState(false);
	}
}

void ULoginWidget::OnLoginFailure(const FString& ErrorMessage)
{
	UE_LOGFMT(LogProjectA, Warning, "{0}: Login failed. Error={1}", FString(__FUNCTION__), *ErrorMessage);

	SetLoadingState(false);

	if (ErrorText)
	{
		ErrorText->SetText(FText::FromString(ErrorMessage));
		ErrorText->SetVisibility(ESlateVisibility::Visible);
	}

	OnLoginAnimationFailure();
}

void ULoginWidget::SetLoadingState(bool bLoading)
{
	if (LoginButton)
	{
		LoginButton->SetIsEnabled(!bLoading);
	}
	if (LoadingOverlay)
	{
		LoadingOverlay->SetVisibility(bLoading ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
	if (ErrorText && bLoading)
	{
		ErrorText->SetVisibility(ESlateVisibility::Collapsed);
	}
}
#include "HealthWidget.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "HealthComponent.h"

#include "ProjectALog.h"

void UHealthWidget::InitialiseWithHealthComponent(UHealthComponent* InHealthComponent)
{
	if (!InHealthComponent)
	{
		UE_LOGFMT(LogProjectA, Error, "{0} - InHealthComponent is nullptr", FString(__FUNCTION__));
		return;
	}

	if (HealthComponent.IsValid())
	{
		HealthComponent->OnHealthChanged.RemoveDynamic(this, &UHealthWidget::HandleHealthChanged);
	}

	HealthComponent = InHealthComponent;
	InHealthComponent->OnHealthChanged.AddDynamic(this, &UHealthWidget::HandleHealthChanged);
	HandleHealthChanged(InHealthComponent->GetCurrentHealth(), InHealthComponent->GetMaxHealth());
}

void UHealthWidget::HandleHealthChanged(float CurrentHealthValue, float MaxHealthValue)
{
	if (!HpProgressBar || !HpText)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - HpProgressBar or HpText is nullptr", FString(__FUNCTION__));
		return;
	}

	float Percent{0.f};

	if (MaxHealthValue > 0.f)
	{
		Percent = CurrentHealthValue / MaxHealthValue;
	}

	HpProgressBar->SetPercent(Percent);
	HpText->SetText(FText::FromString(FString::Printf(TEXT("%.0f / %.0f"), CurrentHealthValue, MaxHealthValue)));
	
}
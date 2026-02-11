/*****************************************************************//**
 * \file   BaseWidget.h
 * \brief  Base class for all widgets in the project.
 * 
 * \date   February 2026
 *********************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "BaseWidget.generated.h"

/**
 * @class UBaseWidget
 * @brief It is a base class for all widgets in the project and can be used to add common functionality to all widgets in the future.
 */
UCLASS()
class UI_API UBaseWidget : public UUserWidget
{
	GENERATED_BODY()
};

// Fill out your copyright notice in the Description page of Project Settings.

#include "FirstPersonWidget.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Components/Image.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Overlay.h"


void UFirstPersonWidget::UpdateInteractPrompt(const FString& NewText)
{
	if (InteractPrompt)
	{
		InteractPrompt->SetText(FText::FromString(NewText));
	}
}

void UFirstPersonWidget::ShowInteractPrompt(bool bShow)
{
	if (InteractPrompt)
	{
		InteractPrompt->SetVisibility(bShow ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}
}

void UFirstPersonWidget::ShowCrosshair(bool bShow)
{
	if (Crosshair)
	{
		Crosshair->SetVisibility(bShow ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}
}

void UFirstPersonWidget::SetCrosshairColor(const FLinearColor& NewColor)
{
	if (Crosshair)
	{
		Crosshair->SetColorAndOpacity(NewColor);
	}
}

void UFirstPersonWidget::UpdateEnterPrompt(const FString& NewText)
{
	if (EnterPrompt)
	{
		EnterPrompt->SetText(FText::FromString(NewText));
	}
}

void UFirstPersonWidget::ShowEnterPrompt(bool bShow)
{
	if (EnterPrompt)
	{
		EnterPrompt->SetVisibility(bShow ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}
}


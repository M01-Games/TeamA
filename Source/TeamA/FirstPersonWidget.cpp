// Fill out your copyright notice in the Description page of Project Settings.

#include "FirstPersonWidget.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Components/Image.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Widget.h"
#include "Components/WidgetComponent.h"
#include "Fonts/SlateFontInfo.h"
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

void UFirstPersonWidget::ShowControlModeIndicator(bool bShow)
{
	if (ControlModeIndicator)
	{
		ControlModeIndicator->SetVisibility(bShow ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}
}

void UFirstPersonWidget::SetControlModeIndicatorKBM(bool bIsKBM)
{
	if (ControlModeIndicator)
	{
		// Assuming you have two different textures for KBM and Gamepad, you can set them here
		UTexture2D* NewTexture = bIsKBM ? LoadObject<UTexture2D>(nullptr, TEXT("/Game/Assets/UI/Button_Icons/keyboard.keyboard")) : LoadObject<UTexture2D>(nullptr, TEXT("/Game/Assets/UI/Button_Icons/controller_switch_pro.controller_switch_pro"));
		ControlModeIndicator->SetBrushFromTexture(NewTexture);
	}
}

void UFirstPersonWidget::SetPromptFontsKB(bool bIsKBM)
{
	if (InteractPrompt)
	{
		InteractPrompt->SetFont(bIsKBM ? KBMFontInfo : GamepadFontInfo);
	}
	if (EnterPrompt)
	{
		EnterPrompt->SetFont(bIsKBM ? KBMFontInfo : GamepadFontInfo);
	}
}
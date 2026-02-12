// Fill out your copyright notice in the Description page of Project Settings.


#include "EnchantingWidget.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Components/Image.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Overlay.h"
#include "Components/CanvasPanel.h"

void UEnchantingWidget::UpdateEnchantingPrompt(const FString& NewText)
{
	if (EnchantingPrompt)
	{
		EnchantingPrompt->SetText(FText::FromString(NewText));
	}
}

void UEnchantingWidget::ShowEnchantingPrompt(bool bShow)
{
	if (EnchantingPrompt)
	{
		EnchantingPrompt->SetVisibility(bShow ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}
}
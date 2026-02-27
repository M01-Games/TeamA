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

void UEnchantingWidget::SetEnscribedImage(int32 Index, char RuneType)
{
	UImage* TargetImage = nullptr;
	switch (Index)
	{
	case 0:
		TargetImage = Enscribed1;
		break;
	case 1:
		TargetImage = Enscribed2;
		break;
	case 2:
		TargetImage = Enscribed3;
		break;
	default:
		return; // Invalid index
	}
	if (TargetImage)
	{
		FString TexturePath = FString::Printf(TEXT("/Game/Assets/UI/%c_RUNE.%c_RUNE"), RuneType, RuneType);
		UTexture2D* RuneTexture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), nullptr, *TexturePath));
		if (RuneTexture)
		{
			TargetImage->SetBrushFromTexture(RuneTexture);
			TargetImage->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to load texture for rune type: %c"), RuneType);
			TargetImage->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void UEnchantingWidget::ClearEnscribedImages()
{
	TArray<UImage*> EnscribedImages = { Enscribed1, Enscribed2, Enscribed3 };
	for (UImage* Image : EnscribedImages)
	{
		if (Image)
		{
			Image->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}
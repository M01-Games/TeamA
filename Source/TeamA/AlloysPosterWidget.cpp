// Fill out your copyright notice in the Description page of Project Settings.


#include "AlloysPosterWidget.h"

void UAlloysPosterWidget::SetPosterImage(const FString& PosterImage, bool visible)
{
	UImage* TargetImage = nullptr;

	if (PosterImage.Equals(TEXT("steel"), ESearchCase::IgnoreCase))
	{
		TargetImage = SteelImage;
	}
	else if (PosterImage.Equals(TEXT("brass"), ESearchCase::IgnoreCase))
	{
		TargetImage = BrassImage;
	}
	else if (PosterImage.Equals(TEXT("rosegold"), ESearchCase::IgnoreCase))
	{
		TargetImage = RoseGoldImage;
	}


	if (TargetImage)
	{
		TargetImage->SetVisibility(visible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}
}
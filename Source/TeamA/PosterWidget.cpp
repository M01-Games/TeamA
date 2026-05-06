// Fill out your copyright notice in the Description page of Project Settings.


#include "PosterWidget.h"

void UPosterWidget::SetPosterImage(const FString& PosterImage, bool visible)
{
	UImage* TargetImage = nullptr;

	if (PosterImage.Equals(TEXT("fire"), ESearchCase::IgnoreCase))
	{
		TargetImage = FireImage;
	}
	else if (PosterImage.Equals(TEXT("ice"), ESearchCase::IgnoreCase))
	{
		TargetImage = IceImage;
	}
	else if (PosterImage.Equals(TEXT("lightning"), ESearchCase::IgnoreCase))
	{
		TargetImage = LightningImage;
	}
	else if (PosterImage.Equals(TEXT("poison"), ESearchCase::IgnoreCase))
	{
		TargetImage = PoisonImage;
	}
	else if (PosterImage.Equals(TEXT("light"), ESearchCase::IgnoreCase))
	{
		TargetImage = LightImage;
	}
	else if (PosterImage.Equals(TEXT("dark"), ESearchCase::IgnoreCase))
	{
		TargetImage = DarkImage;
	}
	else if (PosterImage.Equals(TEXT("air"), ESearchCase::IgnoreCase))
	{
		TargetImage = AirImage;
	}
	else if (PosterImage.Equals(TEXT("magic"), ESearchCase::IgnoreCase))
	{
		TargetImage = MagicImage;
	}
	else if (PosterImage.Equals(TEXT("luck"), ESearchCase::IgnoreCase))
	{
		TargetImage = LuckImage;
	}

	if (TargetImage)
	{
		TargetImage->SetVisibility(visible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}
}
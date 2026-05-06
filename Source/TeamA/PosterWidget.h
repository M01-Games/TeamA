// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "PosterWidget.generated.h"

/**
 * 
 */
UCLASS()
class TEAMA_API UPosterWidget : public UUserWidget
{
	GENERATED_BODY()
public:


	// Fire
	UPROPERTY(meta = (BindWidget))
	UImage* FireImage;


	// Ice
	UPROPERTY(meta = (BindWidget))
	UImage* IceImage;


	// Lightning
	UPROPERTY(meta = (BindWidget))
	UImage* LightningImage;


	// Poison
	UPROPERTY(meta = (BindWidget))
	UImage* PoisonImage;


	// Light
	UPROPERTY(meta = (BindWidget))
	UImage* LightImage;


	// Dark
	UPROPERTY(meta = (BindWidget))
	UImage* DarkImage;


	// Air
	UPROPERTY(meta = (BindWidget))
	UImage* AirImage;


	// Magic
	UPROPERTY(meta = (BindWidget))
	UImage* MagicImage;


	// Luck
	UPROPERTY(meta = (BindWidget))
	UImage* LuckImage;


	UFUNCTION(BlueprintCallable)
	void SetPosterImage(const FString& PosterImage, bool visible);
};


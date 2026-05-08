// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "AlloysPosterWidget.generated.h"

/**
 * 
 */
UCLASS()
class TEAMA_API UAlloysPosterWidget : public UUserWidget
{
	GENERATED_BODY()
public:


	// Fire
	UPROPERTY(meta = (BindWidget))
	UImage* BrassImage;


	// Ice
	UPROPERTY(meta = (BindWidget))
	UImage* RoseGoldImage;


	// Lightning
	UPROPERTY(meta = (BindWidget))
	UImage* SteelImage;

	UFUNCTION(BlueprintCallable)
	void SetPosterImage(FString& PosterImage, bool visible);
};

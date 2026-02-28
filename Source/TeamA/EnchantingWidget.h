// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Fonts/SlateFontInfo.h"
#include "EnchantingWidget.generated.h"

/**
 * 
 */
UCLASS()
class TEAMA_API UEnchantingWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(meta = (BindWidget))
	UImage* RuneImage;

	UPROPERTY(meta = (BindWidget))
	UImage* Enscribed1;

	UPROPERTY(meta = (BindWidget))
	UImage* Enscribed2;

	UPROPERTY(meta = (BindWidget))
	UImage* Enscribed3;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* EnchantingPrompt;



	void UpdateEnchantingPrompt(const FString& NewText);
	void SetPromptFontsKB(bool bIsKBM);

	UPROPERTY(EditAnywhere, Category = "Appearance")
	FSlateFontInfo KBMFontInfo;
	UPROPERTY(EditAnywhere, Category = "Appearance")
	FSlateFontInfo GamepadFontInfo;

	void ShowEnchantingPrompt(bool bShow);


	void SetEnscribedImage(int32 Index, char RuneType);

	void ClearEnscribedImages();

};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Fonts/SlateFontInfo.h"
#include "FirstPersonWidget.generated.h"



/**
 * 
 */
UCLASS()
class TEAMA_API UFirstPersonWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	class UImage* Crosshair;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* InteractPrompt;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* EnterPrompt;

	UPROPERTY(meta = (BindWidget))
	class UImage* ControlModeIndicator;

	void UpdateInteractPrompt(const FString& NewText);
	void ShowInteractPrompt(bool bShow);

	void ShowCrosshair(bool bShow);
	void SetCrosshairColor(const FLinearColor& NewColor);

	void UpdateEnterPrompt(const FString& NewText);
	void ShowEnterPrompt(bool bShow);

	void ShowControlModeIndicator(bool bShow);
	void SetControlModeIndicatorKBM(bool bIsKBM);

	void SetPromptFontsKB(bool bIsKBM);

	UPROPERTY(EditAnywhere, Category = "Appearance")
	FSlateFontInfo KBMFontInfo;
	UPROPERTY(EditAnywhere, Category = "Appearance")
	FSlateFontInfo GamepadFontInfo;
};

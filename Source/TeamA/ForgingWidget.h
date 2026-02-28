// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ForgingWidget.generated.h"

/**
 * 
 */
UCLASS()
class UForgingWidget : public UUserWidget
{
    GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	class UCanvasPanel* Canvas;
	FVector2D GetCanvasSize() const;


    UPROPERTY(meta = (BindWidget))
    class UTextBlock* ForgePrompt;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* MovePrompt;

	UPROPERTY(meta = (BindWidget))
	class UImage* ForgeTarget;

	UPROPERTY(meta = (BindWidget))
	class UImage* GoodZone;

	UPROPERTY(meta = (BindWidget))
	class UImage* PerfectZone;

	void UpdateForgePrompt(const FString& NewText);
	void ShowForgePrompt(bool bShow);

	void UpdateMovePrompt(const FString& NewText);
	void ShowMovePrompt(bool bShow);

	void SetForgeTargetPercent(float NewPercent);
	void SetForgeTargetVisible(bool bVisible);

	void UpdateHammerTimingZones(float Target, float PerfectThreshold, float GoodThreshold);

	void SetPromptFontsKB(bool bIsKBM);

	UPROPERTY(EditAnywhere, Category = "Appearance")
	FSlateFontInfo KBMFontInfo;
	UPROPERTY(EditAnywhere, Category = "Appearance")
	FSlateFontInfo GamepadFontInfo;
};

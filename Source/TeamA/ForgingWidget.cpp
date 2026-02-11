// Fill out your copyright notice in the Description page of Project Settings.

#include "ForgingWidget.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Components/Image.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Overlay.h"
#include "Components/CanvasPanel.h"


void UForgingWidget::UpdateForgePrompt(const FString& NewText)
{
	if (ForgePrompt)
	{
		ForgePrompt->SetText(FText::FromString(NewText));
	}
}

void UForgingWidget::ShowForgePrompt(bool bShow)
{
	if (ForgePrompt)
	{
		ForgePrompt->SetVisibility(bShow ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}
}


// ===== Hammer Bars =====

void UForgingWidget::UpdateHammerBar_0(float Progress)
{
	if (HammerBar_0)
	{
		HammerBar_0->SetPercent(Progress);
	}
}



void UForgingWidget::ShowHammerBar_0(bool bShow)
{
	if (HammerBar_0)
	{
		HammerBar_0->SetVisibility(bShow ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}
}


void UForgingWidget::SetHammerBar_0Color(const FLinearColor& NewColor)
{
	if (HammerBar_0)
	{

	}
}


void UForgingWidget::SetHammerBar_0Target(float NewTarget)
{
	// Implementation depends on how the target is represented in the UI.
	// This is a placeholder for setting a target marker on the progress bar.
}




void UForgingWidget::SetHammerBar_0Position(float x, float y)
{
	if (HammerBar_0)
	{
		UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(HammerBar_0->Slot);
		if (CanvasSlot)
		{
			CanvasSlot->SetPosition(FVector2D(x, y));
		}
	}
}





FVector2D UForgingWidget::GetCanvasSize() const
{
	// Return the resolution of the canvas panel
	if (Canvas)
	{
		return Canvas->GetCachedGeometry().GetLocalSize();
	}
	return FVector2D::ZeroVector;
}

void UForgingWidget::SetForgeTargetPercent(float NewPercent)
{
	if (ForgeTarget)
	{
		//Get size of parent widget of ForgeTarget
		if (ForgeTarget->GetParent())
		{
			UOverlay* ParentOverlay = Cast<UOverlay>(ForgeTarget->GetParent());
			//Get size of parent overlay
			if (ParentOverlay)
			{
				FVector2D ParentSize = ParentOverlay->GetCachedGeometry().GetLocalSize();
				//Calculate new position based on percent
				float NewY = -(ParentSize.Y * (NewPercent-0.5));
				//Set transform translation
				ForgeTarget->SetRenderTranslation(FVector2D(0.0f, NewY));

			}
		}
	}
}

void UForgingWidget::SetForgeTargetVisible(bool bVisible)
{
	if (ForgeTarget)
	{
		ForgeTarget->SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}
}
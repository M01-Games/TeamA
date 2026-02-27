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
	if(GoodZone)
	{
		GoodZone->SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}
	if(PerfectZone)
	{
		PerfectZone->SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}
}


void UForgingWidget::UpdateHammerTimingZones(
	float Target,
	float PerfectThreshold,
	float GoodThreshold
)
{


	// Set the position of both zones to be centered on the target
	if (PerfectZone && GoodZone)
	{
		if (PerfectZone->GetParent())
		{
			UOverlay* ParentOverlay = Cast<UOverlay>(PerfectZone->GetParent());
			if (ParentOverlay)
			{
				FVector2D ParentSize = ParentOverlay->GetCachedGeometry().GetLocalSize();
				float NewY = -(ParentSize.Y * (Target - 0.5f));
				PerfectZone->SetRenderTranslation(FVector2D(0.0f, NewY));
				PerfectZone->SetDesiredSizeOverride(FVector2D(PerfectZone->GetDesiredSize().X, ParentSize.Y * PerfectThreshold));
			}
		}
		if (GoodZone->GetParent())
		{
			UOverlay* ParentOverlay = Cast<UOverlay>(GoodZone->GetParent());
			if (ParentOverlay)
			{
				FVector2D ParentSize = ParentOverlay->GetCachedGeometry().GetLocalSize();
				float NewY = -(ParentSize.Y * (Target - 0.5f));
				GoodZone->SetRenderTranslation(FVector2D(0.0f, NewY));
				GoodZone->SetDesiredSizeOverride(FVector2D(GoodZone->GetDesiredSize().X, ParentSize.Y * GoodThreshold));
			}
		}

		
	}
}

// Fill out your copyright notice in the Description page of Project Settings.

#include "ForgingStation.h"
#include "InputActionValue.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Character.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "DrawDebugHelpers.h"
#include "Project.h"
#include "NiagaraSystem.h"
#include "ForgingTargetActor.h"
#include "Components/WidgetComponent.h"
#include <iostream>
#include <algorithm>
#include "ForgingWidget.h"
using namespace std;


AForgingStation::AForgingStation()
{
	

	isEntered = false;
}

void AForgingStation::BeginPlay()
{
	Super::BeginPlay();

}

// Enter and Exit

void AForgingStation::Enter_Implementation(ACharacter* Character)
{

	// Ensure we have a valid local player controller and bind input
	APlayerController* PC = Character
		? Cast<APlayerController>(Character->GetController())
		: nullptr;

	if (!PC || !PC->IsLocalController())
	{
		return;
	}


	BindInput(PC);



	// Get current project by checking the first project in the workstation's inventory
	CurrentProject = nullptr;
	if (Inventory.Num() > 0)
	{
		CurrentProject = Cast<AProject>(Inventory[0]);
	}

	if (!CurrentProject)
	{
		UE_LOG(LogTemp, Warning, TEXT("ForgingStation: No current project found in inventory"));
	}
	else 
	{
		CurrentForgingPattern = CurrentProject->ForgingPattern;
	}



	isEntered = true;
	isForging = false;
	PrimaryActorTick.bCanEverTick = true;

	CurrentTargetValue = 0.0f;


	if (ForgingWidgetClass)
	{
		ForgingWidgetInstance = CreateWidget<UForgingWidget>(
			GetWorld(),
			ForgingWidgetClass
		);

		if (ForgingWidgetInstance)
		{
			ForgingWidgetInstance->AddToViewport();
		}

		if(!CurrentProject)
		{
			ForgingWidgetInstance->UpdateForgePrompt(TEXT("Insert a project to begin"));
		}
		else if(CurrentProject->bIsForged)
		{
			ForgingWidgetInstance->UpdateForgePrompt(TEXT("Project already forged"));
		}
		else {
			ForgingWidgetInstance->UpdateForgePrompt(TEXT("Press Space to start forging"));
		}


		ForgingWidgetInstance->ShowForgePrompt(true);

		ForgingWidgetInstance->ShowHammerBar_0(false); 
	}
}


void AForgingStation::Exit_Implementation(ACharacter* Character)
{
	APlayerController* PC = Character
		? Cast<APlayerController>(Character->GetController())
		: nullptr;

	if (!PC || !PC->IsLocalController())
	{
		return;
	}


	UnbindInput();


	FInputModeGameOnly InputMode;
	PC->SetInputMode(InputMode);


	isEntered = false;
	PrimaryActorTick.bCanEverTick = false;

	// Unload forging UI
	if (ForgingWidgetInstance)
	{
		ForgingWidgetInstance->RemoveFromParent();
		ForgingWidgetInstance = nullptr;

		/*
		if (ActiveTargets.Num() > 0)
		{
			// Clear targets
			for (AForgingTargetActor* Target : ActiveTargets)
			{
				if (Target)
					Target->Destroy();
			}
		}*/

	}
}


void AForgingStation::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!isEntered)
	{
		return;
	}

	if (CurrentTargetValue <= 0) {
		ForgingWidgetInstance->SetForgeTargetVisible(false);
	}
	else {
		ForgingWidgetInstance->SetForgeTargetVisible(true);
	}

	// Implementation for starting the forging sequence
	if (CurrentProject)
	{
		if (CurrentProject->bIsForged)
		{
			// Already forged
			return;
		}

		if (isForging)
		{
			float FillSpeed = 1.0f / HammerFillDuration;


			TimingPerfectThreshold = TimingPerfectThresholdDefault - (2.0f / 3.0f * TimingPerfectThresholdDefault * ((100.0f - CurrentProject->HeatIntensity) / 100.0f));
			TimingGoodThreshold = TimingGoodThresholdDefault - (2.0f / 3.0f * TimingGoodThresholdDefault * ((100.0f - CurrentProject->HeatIntensity) / 100.0f));

			CurrentHammerFill += DeltaTime * FillSpeed;
			CurrentHammerFill = FMath::Clamp(CurrentHammerFill, -HammerFillDelay, 1.0f);
			CurrentTargetValue = CurrentForgingPattern.IsValidIndex(CurrentHammerIndex)
				? CurrentForgingPattern[CurrentHammerIndex]
				: 0.5f;

			// Update UI
			ForgingWidgetInstance->UpdateHammerBar_0( max(CurrentHammerFill, 0.0f));
			ForgingWidgetInstance->SetForgeTargetPercent(CurrentTargetValue);

			ForgingWidgetInstance->UpdateHammerTimingZones(
				CurrentTargetValue,
				TimingPerfectThreshold,
				TimingGoodThreshold
			);
			
			AForgingTargetActor* CurrentTarget = ActiveTargets.IsValidIndex(CurrentHammerIndex)
				? ActiveTargets[CurrentHammerIndex]
				: nullptr;
			if (CurrentTarget) {
				//Color change logic here
				CurrentTarget->SetTargetWidgetColor(FLinearColor::Green);
			}

		}

		if (isForging)
		{
			UpdateIndicatorPosition(DeltaTime);

			SetHammerPosition(
				GetIndicatorWorldPosition(IndicatorAlpha)
			);


		}
	}

}

void AForgingStation::OnHammerPressed()
{
	// If we are forging, hammer ALWAYS wins
	if (isForging)
	{
		ProcessHammerInput();
		return;
	}

	// Only allow starting forging if NOT forging
	StartForgingSequence();
}


void AForgingStation::StartForgingSequence()
{
	UE_LOG(LogTemp, Warning, TEXT("StartForgingSequence called"));

	if (isForging)
	{
		return;
	}
	if (!CurrentProject)
	{
		UE_LOG(LogTemp, Warning, TEXT("No project to forge"));
		return;
	}
	if (CurrentProject->bIsForged)
	{
		UE_LOG(LogTemp, Warning, TEXT("Project already forged"));
		return;
	}

	if (CurrentProject->forgingProgress >= 1.0f)
	{
		CurrentProject->bIsForged = true;
		ForgingWidgetInstance->UpdateForgePrompt(TEXT("Project fully forged"));
		ForgingWidgetInstance->ShowForgePrompt(true);
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("FORGING"));
	ForgingWidgetInstance->ShowForgePrompt(false);

	//Create an array of floats equal to the forging pattern length
	TArray<float> TargetPositions;
	int PatternLength = CurrentForgingPattern.Num();

	// HERE
	USkeletalMeshComponent* BladeMesh = CurrentProject->SkeletalMesh;
	if (!BladeMesh || !TargetActorClass)
		return;

		
	ActiveTargets.Empty();

	
	USceneComponent* HandleLocation = CurrentProject->HandleSide;
	USceneComponent* TipLocation = CurrentProject->TipSide;



	if (!HandleLocation || !TipLocation || !TargetActorClass)
		return;


	FVector HandlePoint = HandleLocation->GetComponentLocation();
	FVector TipPoint = TipLocation->GetComponentLocation();

	CachedHandlePoint = HandlePoint;
	CachedTipPoint = TipPoint;

	// Clear old targets
	for (AForgingTargetActor* Target : ActiveTargets)
	{
		if (Target)
			Target->Destroy();
	}
	ActiveTargets.Empty();

	// Direction along blade
	FVector BladeDirection = (HandlePoint - TipPoint).GetSafeNormal();
	float BladeLength = FVector::Distance(TipPoint, HandlePoint);

	// Generate sorted alphas (0–1 along blade)
	TArray<float> Alphas;
	for (int32 i = 0; i < PatternLength; i++)
	{
		Alphas.Add(FMath::FRandRange(0.1f, 0.9f));
	}
	Alphas.Sort(); 
	// Reverse alphas so that targets spawn from tip to handle (matches forging direction)
	Algo::Reverse(Alphas);

	for (int32 i = 0; i < PatternLength; i++)
	{
		float Alpha = Alphas[i];

		// World-space position along blade
		FVector WorldPos =
			FMath::Lerp(HandlePoint, TipPoint, Alpha);

		AForgingTargetActor* Target =
			GetWorld()->SpawnActor<AForgingTargetActor>(
				TargetActorClass,
				WorldPos,
				BladeMesh->GetComponentRotation()
			);

		if (!Target)
			continue;

		Target->AttachToComponent(
			BladeMesh,
			FAttachmentTransformRules::KeepWorldTransform
		);

		// 🔑 FIX SCALE INHERITANCE
		Target->SetActorRelativeScale3D(FVector::OneVector);

		// Optional surface offset
		Target->AddActorLocalOffset(FVector(15.f, 5.f, 0.f));

		ActiveTargets.Add(Target);
		TargetAlphas.Add(Alpha);
	}


	IndicatorAlpha = 0.5f; // Set FIRST

	if (!IndicatorActor && IndicatorActorClass)
	{
		IndicatorActor = GetWorld()->SpawnActor<AActor>(
			IndicatorActorClass,
			FTransform::Identity
		);
	}


	ForgingWidgetInstance->ShowHammerBar_0(true);
	ForgingWidgetInstance->UpdateHammerBar_0(0.0f);

	isForging = true;
	CurrentHammerIndex = 0;
	CurrentHammerFill = -HammerFillDelay;
	TotalHammerHits = CurrentForgingPattern.Num();

	// For now, fixed target
	CurrentTargetValue = CurrentForgingPattern[CurrentHammerIndex];

	BeginNextHammer();
	
}

void AForgingStation::ProcessHammerInput()
{
	UE_LOG(LogTemp, Warning, TEXT("ProcessHammerInput called"));
	if (!isForging || !CurrentProject || !ForgingWidgetInstance)
		return;


	APlayerController* PC = Cast<APlayerController>(GetWorld()->GetFirstPlayerController());


	AForgingTargetActor* CurrentTarget = ActiveTargets.IsValidIndex(CurrentHammerIndex)
		? ActiveTargets[CurrentHammerIndex]
		: nullptr;


	if (!CurrentTarget)
		return;




	// Evaluate scores
	EForgeHitQuality TimingQuality =
		EvaluateTiming(CurrentHammerFill, CurrentTargetValue);

	EForgeHitQuality PositionQuality = 
		EvaluateIndicatorPosition(CurrentHammerIndex);


	EForgeHitQuality FinalQuality =
		CombineHitQuality(TimingQuality, PositionQuality);


	//PlayHammerAnimation(HitWorldPos, FinalQuality);
	PlayHammerAnimation(CurrentHammerIndex, IndicatorActor ? IndicatorActor->GetActorLocation() : FVector::ZeroVector);


	UE_LOG(
		LogTemp,
		Warning,
		TEXT("Hammer %d hit at %.2f"),
		CurrentHammerIndex,
		CurrentHammerFill
	);

	// Apply forging progress
	CurrentProject->forgingProgress += ForgingProgressPerHit;
	CurrentProject->forgingProgress =
		FMath::Clamp(CurrentProject->forgingProgress, 0.0f, 1.0f);
	CurrentProject->ForgeModel();

	float EffectScale = 1.0f;

	switch (FinalQuality)
	{
	case EForgeHitQuality::Perfect:
		EffectScale = EffectScale * 1.3f;
		break;

	case EForgeHitQuality::Good:
		EffectScale *= 1.0f;
		break;

	case EForgeHitQuality::Bad:
		EffectScale *= 0.7f;
		//Find the Grain Strength parameter of the current project and increase it by a small amount as a penalty for bad hits
		CurrentProject->GrainStrength += 0.05f;
		UE_LOG(LogTemp, Warning, TEXT("Grain Strength increased to %.2f"), CurrentProject->GrainStrength);



		break;
	}

	UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		GetWorld(), //Spawn in world
		HitEffect, // Effect to spawn
		IndicatorActor ? IndicatorActor->GetActorLocation() : FVector::ZeroVector, // Spawn at indicator location
		FRotator::ZeroRotator, // No rotation
		FVector(EffectScale), // Scale based on hit quality
		true, // Auto destroy
		true // Auto activate
	);



	// Hide current target
	CurrentTarget->Destroy();

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("Hit %d - Timing: %d | Position: %d | Final: %d | Progress: %.0f%%"),
		CurrentHammerIndex,
		(int32)TimingQuality,
		(int32)PositionQuality,
		(int32)FinalQuality,
		CurrentProject->forgingProgress * 100.0f
	);




	float HitScore = 0.0f;

	switch (FinalQuality)
	{
	case EForgeHitQuality::Perfect:
		HitScore = PerfectHitScore;
		break;

	case EForgeHitQuality::Good:
		HitScore = GoodHitScore;
		break;

	case EForgeHitQuality::Bad:
		HitScore = BadHitScore;
		break;
	}

	CurrentProject->TotalForgeHits++;
	CurrentProject->TotalForgeScore += HitScore;


	CurrentHammerIndex++;
	BeginNextHammer();
}



void AForgingStation::BeginNextHammer()
{
	if (CurrentHammerIndex >= TotalHammerHits)
	{
		FinishForging();
		return;
	}

	CurrentHammerFill = -HammerFillDelay;

	ForgingWidgetInstance->ShowHammerBar_0(true);
}

void AForgingStation::FinishForging()
{
	isForging = false;
	CurrentTargetValue = 0.0f;

	if (CurrentProject->forgingProgress >= 1.0f)
	{
		CurrentProject->bIsForged = true;
		CurrentProject->FinalizeForgingScore();

		ForgingWidgetInstance->UpdateForgePrompt(
			FString::Printf(
				TEXT("Forging complete! Score: %.0f"),
				CurrentProject->FinalForgeScore
			)
		);
	}
	else
	{
		//Hide hammer bar
		ForgingWidgetInstance->ShowHammerBar_0(false);
		ForgingWidgetInstance->UpdateForgePrompt(TEXT("Press Space to continue forging"));
	}

	ForgingWidgetInstance->ShowForgePrompt(true);

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("Forging sequence complete. Progress: %.0f%%"),
		CurrentProject->forgingProgress * 100.0f
	);

}






// Input Binding

void AForgingStation::BindInput(APlayerController* PC)
{
	if (!PC) return;

	// Enable input gives us access to the PlayerController's InputComponent
	EnableInput(PC);



	CachedEnhancedInput = Cast<UEnhancedInputComponent>(InputComponent);
	if (!CachedEnhancedInput)
	{
		UE_LOG(LogTemp, Error, TEXT("ForgingStation: No EnhancedInputComponent"));
		return;
	}

	CachedEnhancedInput->BindAction(
		HammerAction,
		ETriggerEvent::Started,
		this,
		&AForgingStation::OnHammerPressed
	);

	
	CachedEnhancedInput->BindAction(
		MoveIndicatorLeftAction,
		ETriggerEvent::Triggered,
		this,
		&AForgingStation::MoveIndicatorLeft
	);

	CachedEnhancedInput->BindAction(
		MoveIndicatorRightAction,
		ETriggerEvent::Triggered,
		this,
		&AForgingStation::MoveIndicatorRight
	);
}

void AForgingStation::MoveIndicatorLeft()
{
	if (!isForging) return;

	IndicatorAlpha -= IndicatorSpeed * GetWorld()->GetDeltaSeconds();
	IndicatorAlpha = FMath::Clamp(IndicatorAlpha, 0.0f, 1.0f);
}

void AForgingStation::MoveIndicatorRight()
{
	if (!isForging) return;

	IndicatorAlpha += IndicatorSpeed * GetWorld()->GetDeltaSeconds();
	IndicatorAlpha = FMath::Clamp(IndicatorAlpha, 0.0f, 1.0f);
}

void AForgingStation::UpdateIndicatorPosition(float DeltaTime)
{
	if (!IndicatorActor) return;

	FVector TargetPos = GetIndicatorWorldPosition(IndicatorAlpha);

	// Smooth movement (prevents snapping)
	FVector NewPos = FMath::VInterpTo(
		IndicatorActor->GetActorLocation(),
		TargetPos,
		DeltaTime,
		12.0f // responsiveness
	);

	IndicatorActor->SetActorLocation(NewPos);
}

void AForgingStation::UnbindInput()
{
	if (!CachedEnhancedInput) return;

	CachedEnhancedInput->ClearActionBindings();
	CachedEnhancedInput = nullptr;

	DisableInput(nullptr);
}

EForgeHitQuality AForgingStation::EvaluateTiming(float FillValue, float TargetValue) const
{
	float Error = FMath::Abs(FillValue - TargetValue);

	if (Error <= TimingPerfectThreshold)
		return EForgeHitQuality::Perfect;

	if (Error <= TimingGoodThreshold)
		return EForgeHitQuality::Good;

	return EForgeHitQuality::Bad;
}

FVector AForgingStation::GetIndicatorWorldPosition(float Alpha) const
{
	return FMath::Lerp(CachedTipPoint, CachedHandlePoint, Alpha);
}

EForgeHitQuality AForgingStation::EvaluateIndicatorPosition(int32 TargetIndex) const { 
	if (!TargetAlphas.IsValidIndex(TargetIndex)) 
		return EForgeHitQuality::Bad; 
	
	float TargetAlpha = TargetAlphas[TargetIndex]; 
	float Error = FMath::Abs(IndicatorAlpha - TargetAlpha); 
	UE_LOG(LogTemp, Warning, TEXT("Evaluating position: IndicatorAlpha=%.2f | TargetAlpha=%.2f | Error=%.2f"), IndicatorAlpha, TargetAlpha, Error);
	if (Error <= TimingPerfectThreshold/2) return EForgeHitQuality::Perfect; 
	if (Error <= TimingGoodThreshold/2) return EForgeHitQuality::Good; 
	return EForgeHitQuality::Bad; 
}

EForgeHitQuality AForgingStation::CombineHitQuality(EForgeHitQuality Timing, EForgeHitQuality Position) { return static_cast<EForgeHitQuality>(FMath::Max(static_cast<uint8>(Timing), static_cast<uint8>(Position))); }
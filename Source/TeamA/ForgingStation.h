// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Workstation.h"
#include "Project.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "ForgingWidget.h"
#include "ForgingTargetActor.h"
#include "NiagaraSystem.h"

#include "ForgingStation.generated.h"

/**
 * 
 */

class UInputComonponent;

UENUM(BlueprintType)
enum class EForgeHitQuality : uint8
{
	Perfect,
	Good,
	Bad
};


UCLASS()
class TEAMA_API AForgingStation : public AWorkstation
{
	GENERATED_BODY()

public:
	AForgingStation();

	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* StartForgingAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* HammerAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* MoveIndicatorLeftAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* MoveIndicatorRightAction;

	void StartForgingSequence();
	void ProcessHammerInput();

	// How much forging progress per successful hit
	UPROPERTY(EditAnywhere, Category = "Forging")
	float ForgingProgressPerHit = 0.15f; // 5%

	// Scoring tolerances
	UPROPERTY(EditAnywhere, Category = "Forging|Scoring")
	float TimingPerfectThresholdDefault = 0.15f; // ±5%
	float TimingPerfectThreshold = 0.15f; // ±5%

	UPROPERTY(EditAnywhere, Category = "Forging|Scoring")
	float TimingGoodThresholdDefault = 0.25f; // ±10%
	float TimingGoodThreshold = 0.25f; // ±15%

	UPROPERTY(EditAnywhere, Category = "Forging|Scoring")
	float PositionPerfect = 0.1;

	UPROPERTY(EditAnywhere, Category = "Forging|Scoring")
	float PositionGood = 0.2;

	// Forging score tracking
	UPROPERTY(BlueprintReadOnly)
	int32 TotalForgeHits = 0;

	UPROPERTY(BlueprintReadOnly)
	float TotalForgeScore = 0.0f;

	// Final normalized score (0–100)
	UPROPERTY(BlueprintReadOnly)
	float FinalForgeScore = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Forging|Scoring")
	float PerfectHitScore = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Forging|Scoring")
	float GoodHitScore = 0.6f;

	UPROPERTY(EditAnywhere, Category = "Forging|Scoring")
	float BadHitScore = 0.2f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VFX")
	UNiagaraSystem* HitEffect;


	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AForgingTargetActor> TargetActorClass;
	TArray<AForgingTargetActor*> ActiveTargets;

	EForgeHitQuality CombineHitQuality(EForgeHitQuality Timing, EForgeHitQuality Position);

	// Indicator position along blade (0 = handle, 1 = tip)
	float IndicatorAlpha = 0.5f;

	// How fast indicator moves per second
	UPROPERTY(EditAnywhere, Category = "Forging|Indicator")
	float IndicatorSpeed = 1.0f;

	// Cached blade endpoints (world space)
	FVector CachedHandlePoint;
	FVector CachedTipPoint;

	// Visual indicator actor
	UPROPERTY(EditAnywhere, Category = "Forging|Indicator")
	TSubclassOf<AActor> IndicatorActorClass;

	AActor* IndicatorActor = nullptr;

	EForgeHitQuality EvaluateIndicatorPosition(int32 TargetIndex) const;

protected:
	virtual void BeginPlay() override;

	AProject* CurrentProject;
	
	TArray<float> CurrentForgingPattern;
	
	bool isEntered;
	bool isForging;
	
	// Forging progression
	int32 CurrentHammerIndex = 0;

	// Current bar fill (0–1)
	float CurrentHammerFill = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Forging")
	float HammerFillDuration = 1.f;

	UPROPERTY(EditAnywhere, Category = "Forging")
	float HammerFillDelay = 0.4f;

	// Cached pattern length
	int32 TotalHammerHits = 0;

	// Target value (0–1) — later derived from pattern
	float CurrentTargetValue = 0.5f;


	// Cached target positions (generated at start)
	TArray<float> CachedTargetPositions;

	void BeginNextHammer();
	void FinishForging();
	EForgeHitQuality EvaluateTiming(float FillValue, float TargetValue) const;

	UFUNCTION(BlueprintImplementableEvent, Category = "Forging|Visuals")
	void PlayHammerAnimation(int32 HammerIndex, FVector location);


	UFUNCTION(BlueprintImplementableEvent, Category = "Forging|Visuals")
	void SetHammerPosition(FVector location);

	UPROPERTY()
	UEnhancedInputComponent* CachedEnhancedInput = nullptr;

	void BindInput(APlayerController* PC);
	void UnbindInput();

	void MoveIndicatorLeft();
	void MoveIndicatorRight();
	void UpdateIndicatorPosition(float DeltaTime);

	void OnHammerPressed();

	TArray<float> TargetAlphas;

	// Widget instance (runtime)
	UPROPERTY()
	UForgingWidget* ForgingWidgetInstance;

	FVector GetIndicatorWorldPosition(float Alpha) const;

	bool GoingRight = true;

public:
	virtual void Tick(float DeltaTime) override;
	virtual void Enter_Implementation(class ACharacter* Character) override;
	virtual void Exit_Implementation(class ACharacter* Character) override;


	// UI Widget for forging
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<class UUserWidget> ForgingWidgetClass;
};

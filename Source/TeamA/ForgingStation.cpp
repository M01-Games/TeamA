// Fill out your copyright notice in the Description page of Project Settings.

#include "InputActionValue.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Character.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"
#include "ForgingWidget.h"
#include "ForgingStation.h"

AForgingStation::AForgingStation()
{
	PrimaryActorTick.bCanEverTick = true;

	isEntered = false;


}

void AForgingStation::BeginPlay()
{
	Super::BeginPlay();

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

		ForgingWidgetInstance->UpdateForgePrompt(TEXT("Press Space to start forging"));

		ForgingWidgetInstance->ShowForgePrompt(false);
		ForgingWidgetInstance->ShowHammerBar_0(false);
		ForgingWidgetInstance->ShowTarget_0(false);

	}

}

void AForgingStation::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!isEntered)
	{
		return;
	}

	// Implementation for starting the forging sequence
	if (CurrentProject)
	{
		if (CurrentProject->bIsForged)
		{
			// Already forged
			return;
		}

	}
}

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
		StartForgingAction,
		ETriggerEvent::Triggered,
		this,
		&AForgingStation::StartForgingSequence
	);

	CachedEnhancedInput->BindAction(
		HammerAction,
		ETriggerEvent::Triggered,
		this,
		&AForgingStation::ProcessHammerInput
	);
}

void AForgingStation::UnbindInput()
{
	if (!CachedEnhancedInput) return;

	CachedEnhancedInput->ClearActionBindings();
	CachedEnhancedInput = nullptr;

	DisableInput(nullptr);
}



void AForgingStation::Enter_Implementation(ACharacter* Character)
{
	APlayerController* PC = Character
		? Cast<APlayerController>(Character->GetController())
		: nullptr;

	if (!PC || !PC->IsLocalController())
	{
		return;
	}

	ULocalPlayer* LP = PC->GetLocalPlayer();
	if (!LP) return;

	UEnhancedInputLocalPlayerSubsystem* Subsystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LP);

	if (Subsystem && ForgingMappingContext)
	{
		Subsystem->AddMappingContext(
			ForgingMappingContext,
			ForgingMappingPriority
		);
	}

	BindInput(PC);
	isEntered = true;
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

	ULocalPlayer* LP = PC->GetLocalPlayer();
	if (!LP) return;

	UEnhancedInputLocalPlayerSubsystem* Subsystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LP);

	if (Subsystem && ForgingMappingContext)
	{
		Subsystem->RemoveMappingContext(ForgingMappingContext);
	}

	UnbindInput();
	isEntered = false;
}

void AForgingStation::StartForgingSequence()
{
	UE_LOG(LogTemp, Warning, TEXT("FORGING"));
	ForgingWidgetInstance->ShowForgePrompt(false);

	ForgingWidgetInstance->ShowHammerBar_0(true);
	ForgingWidgetInstance->ShowTarget_0(true);

	//Get canvas size to set target position
	FVector2D ViewportSize = ForgingWidgetInstance->GetCanvasSize();

	// Random target position within the canvas
	float TargetPosition = FMath::FRandRange(0.1f, 0.9f) * ViewportSize.X;
	UE_LOG(LogTemp, Warning, TEXT("Target Position: %f"), TargetPosition);

	ForgingWidgetInstance->SetTarget_0Position(TargetPosition, 500.0f);

}

void AForgingStation::ProcessHammerInput()
{
	// Implementation for processing hammer input
	UE_LOG(LogTemp, Warning, TEXT("HAMMERING"));
	ForgingWidgetInstance->ShowForgePrompt(true);

	// Hide hammer bar and target for now
	ForgingWidgetInstance->ShowHammerBar_0(false);
	ForgingWidgetInstance->ShowTarget_0(false);


}


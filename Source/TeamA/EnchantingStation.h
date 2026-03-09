// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Workstation.h"
#include "InputAction.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Materials/MaterialInterface.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/TextureRenderTarget2D.h"
#include "NiagaraSystem.h"
#include "NNE.h"
#include "NNEModelData.h"
#include "NNERuntimeCPU.h"
#include "EnchantingWidget.h"
#include "Project.h"
#include "Pickup.h"
#include "Crystal.h"
#include "TeamACharacter.h"
#include "ItemTypesEnum.h"
#include "EnchantingStation.generated.h"

UENUM(BlueprintType)
enum class EEnchantingMode : uint8
{
	DatasetBuilder UMETA(DisplayName = "Dataset Builder"),
	GameplayAI       UMETA(DisplayName = "Gameplay AI")
};


class FMyModelHelper
{
public:
	TSharedPtr<UE::NNE::IModelInstanceCPU> ModelInstance;
	TArray<float> InputData;
	TArray<float> OutputData;
	TArray<UE::NNE::FTensorBindingCPU> InputBindings;
	TArray<UE::NNE::FTensorBindingCPU> OutputBindings;
	bool bIsRunning;
};



UCLASS()
class TEAMA_API AEnchantingStation : public AWorkstation
{
	GENERATED_BODY()
	
public:

	static constexpr float CanvasSize = 512.f;

	//Cached Player Controller
	UPROPERTY()
	class APlayerController* CachedPC = nullptr;

	// Draw input action
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* DrawRuneAction;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* FinishRuneAction;


	// Drawing resolution (player-facing)
	UPROPERTY(EditDefaultsOnly, Category = "Rune Drawing")
	int32 RenderTargetSize = 128;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Rune Drawing")
	TSubclassOf<UUserWidget> EnchantingWidgetClass;

	UPROPERTY()
	UEnchantingWidget* EnchantingWidget;


	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Rune Drawing")
	UMaterialInterface* RuneBrushMaterial;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Rune Drawing")
	EEnchantingMode EnchantingMode = EEnchantingMode::GameplayAI;

	UPROPERTY()
	UMaterialInstanceDynamic* RuneBrushMID;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI")
	UMaterialInterface* RuneRenderTargetMaterial;

	UPROPERTY()
	UMaterialInstanceDynamic* RuneRenderTargetMID;

	// Drawing area in screen space
	FVector2D DrawAreaTopLeft;
	FVector2D DrawAreaSize;

	int RuneCounter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	TObjectPtr<UNNEModelData> RuneClassifierModel;

	AProject* CurrentProject;
	ACrystal* Amethyst;

	// Dictionary of encahntments
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enchantments")
	TMap<FString, UMaterialInstance*> EnchantmentMaterials;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enchantments")
	TMap<FString, EEnchants> Enchantments;

	UFUNCTION(BlueprintImplementableEvent)
	void OnEnterBP();

	UFUNCTION(BlueprintImplementableEvent)
	void OnExitBP();

	ATeamACharacter* CachedCharacter = nullptr;

protected:
	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;
	virtual void Enter_Implementation(class ACharacter* PC) override;
	virtual void Exit_Implementation(class ACharacter* PC) override;

	void BindInput(class APlayerController* PC);
	void UnbindInput();

	void ExportRune(const FString& FileName);

	// cached input component
	UPROPERTY()
	class UEnhancedInputComponent* CachedEnhancedInput = nullptr;

	// Start and stop drawing
	void StartDrawing();
	void StopDrawing();
	void FinishRune();

	bool bIsDrawing = false;
	TArray<FVector2D> DrawPoints;
	float MinPointDistance = 3.f; // Minimum distance between points to be added to the stroke

	// Render target used for rune drawing
	UPROPERTY()
	UTextureRenderTarget2D* RuneRenderTarget;



	void CreateRuneRenderTarget();
	void ClearRuneRenderTarget();
	void CreateEnchantingWidget();
	void DrawBrushAtUV(const FVector2D& UV);
	bool GetMouseUV(FVector2D& OutUV) const;

	
	FVector2D LastUV; // Track previous mouse position for smooth strokes
	bool bHasLastUV = false; // Track if we have a valid previous position

	UPROPERTY()
	UImage* RuneImageWidget = nullptr;

	// One stroke = array of UV points
	TArray<FVector2D> CurrentStroke;

	// All strokes for this rune
	TArray<TArray<FVector2D>> RuneStrokes;


	// AI Class labels
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	TArray<FString> RuneClasses;

	TSharedPtr<FMyModelHelper> ModelHelper;

	bool PrepareInputTensor();
	FString ClassifyRune();

	// Add this method declaration
	void OnRuneClassified(const FString& RuneName);


	void UpdateUI();
};

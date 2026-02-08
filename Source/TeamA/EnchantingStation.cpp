// EnchantingStation.cpp
#include "EnchantingStation.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/PlayerController.h"
#include "Components/Image.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Math/Vector2D.h"
#include "GameFramework/Character.h"
#include "Framework/Application/SlateApplication.h"
#include "Engine/GameViewportClient.h"
#include "Engine/Engine.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Slate/SceneViewport.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "Modules/ModuleManager.h"
#include "NNE.h"
#include "NNEModelData.h"
#include "NNERuntime.h"
#include "NNERuntimeCPU.h"
#include "NNETypes.h"
#include "ImageUtils.h"
#include "Interfaces/IPluginManager.h"
#include "UObject/ConstructorHelpers.h"
#include "Materials/MaterialInterface.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/AssetManager.h"
#include "NNERuntimeRunSync.h"
#include "DrawDebugHelpers.h"




void AEnchantingStation::Enter_Implementation(ACharacter* Character)
{
    APlayerController* PC = Character ? Cast<APlayerController>(Character->GetController()) : nullptr;
    if (!PC || !PC->IsLocalController()) return;

    CachedPC = PC;

    // Create brush material
    if (RuneBrushMaterial)
    {
        RuneBrushMID = UMaterialInstanceDynamic::Create(RuneBrushMaterial, this);
    }

    // Create render target
    CreateRuneRenderTarget();
    ClearRuneRenderTarget();

    // Create render target material (to display on UImage)
    if (RuneRenderTargetMaterial)
    {
        RuneRenderTargetMID = UMaterialInstanceDynamic::Create(RuneRenderTargetMaterial, this);
        if (RuneRenderTargetMID && RuneRenderTarget)
        {
            RuneRenderTargetMID->SetTextureParameterValue(TEXT("RuneTexture"), RuneRenderTarget);
        }
    }

    // Bind input
    BindInput(PC);

    // Show cursor
    PC->bShowMouseCursor = true;
    FInputModeGameAndUI InputMode;
    InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
    InputMode.SetHideCursorDuringCapture(false);
    PC->SetInputMode(InputMode);

    PrimaryActorTick.bCanEverTick = true;

    CreateEnchantingWidget();


    
    // Setup AI model if in GameplayAI mode
    if (EnchantingMode == EEnchantingMode::GameplayAI && RuneClassifierModel)
    {
        UE_LOG(LogTemp, Log, TEXT("Initializing AI model..."));

        TWeakInterfacePtr<INNERuntimeCPU> Runtime =
            UE::NNE::GetRuntime<INNERuntimeCPU>(TEXT("NNERuntimeORTCpu"));

        if (!Runtime.IsValid())
        {
            UE_LOG(LogTemp, Error, TEXT("NNERuntimeORTCpu not available"));
            return;
        }

        ModelHelper = MakeShared<FMyModelHelper>();

        // 1. Create CPU model
        TSharedPtr<UE::NNE::IModelCPU> Model =
            Runtime->CreateModelCPU(RuneClassifierModel);

        if (!Model.IsValid())
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to create CPU model"));
            ModelHelper.Reset();
            return;
        }

        ModelHelper->ModelInstance = Model->CreateModelInstanceCPU();

        if (!ModelHelper->ModelInstance.IsValid())
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to create CPU model instance"));
            ModelHelper.Reset();
            return;
        }


        // 2. Create model instance
        ModelHelper->ModelInstance = Model->CreateModelInstanceCPU();

        if (!ModelHelper->ModelInstance.IsValid())
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to create CPU model instance"));
            ModelHelper.Reset();
            return;
        }

        // --- INPUT SHAPE ---
        TConstArrayView<UE::NNE::FTensorDesc> InputTensorDescs =
            ModelHelper->ModelInstance->GetInputTensorDescs();

        checkf(InputTensorDescs.Num() == 1, TEXT("Only supports single input tensor"));

        UE::NNE::FSymbolicTensorShape SymbolicInputShape =
            InputTensorDescs[0].GetShape();

        checkf(SymbolicInputShape.IsConcrete(),
            TEXT("Input tensor must have concrete dimensions"));

        TArray<UE::NNE::FTensorShape> InputShapes =
        {
            UE::NNE::FTensorShape::MakeFromSymbolic(SymbolicInputShape)
        };

        ModelHelper->ModelInstance->SetInputTensorShapes(InputShapes);

        // --- OUTPUT SHAPE ---
        TConstArrayView<UE::NNE::FTensorDesc> OutputTensorDescs =
            ModelHelper->ModelInstance->GetOutputTensorDescs();

        checkf(OutputTensorDescs.Num() == 1, TEXT("Only supports single output tensor"));

        UE::NNE::FSymbolicTensorShape SymbolicOutputShape =
            OutputTensorDescs[0].GetShape();

        checkf(SymbolicOutputShape.IsConcrete(),
            TEXT("Output tensor must have concrete dimensions"));

        TArray<UE::NNE::FTensorShape> OutputShapes =
        {
            UE::NNE::FTensorShape::MakeFromSymbolic(SymbolicOutputShape)
        };

        // --- BUFFERS ---
        ModelHelper->InputData.SetNumZeroed(InputShapes[0].Volume());
        ModelHelper->InputBindings.SetNumZeroed(1);
        ModelHelper->InputBindings[0].Data = ModelHelper->InputData.GetData();
        ModelHelper->InputBindings[0].SizeInBytes =
            ModelHelper->InputData.Num() * sizeof(float);

        ModelHelper->OutputData.SetNumZeroed(OutputShapes[0].Volume());
        ModelHelper->OutputBindings.SetNumZeroed(1);
        ModelHelper->OutputBindings[0].Data = ModelHelper->OutputData.GetData();
        ModelHelper->OutputBindings[0].SizeInBytes =
            ModelHelper->OutputData.Num() * sizeof(float);

        ModelHelper->bIsRunning = false;

        UE_LOG(LogTemp, Log, TEXT("AI model initialized successfully"));
    }
}

bool AEnchantingStation::PrepareInputTensor()
{
    if (!RuneRenderTarget || !ModelHelper.IsValid() || !ModelHelper->ModelInstance.IsValid())
        return false;

    FTextureRenderTargetResource* RTResource = RuneRenderTarget->GameThread_GetRenderTargetResource();
    if (!RTResource) return false;

    // 1. Read pixels
    TArray<FColor> Bitmap;
    if (!RTResource->ReadPixels(Bitmap)) return false;

    int32 Width = RuneRenderTarget->SizeX;
    int32 Height = RuneRenderTarget->SizeY;

    float IMAGE_SIZE = RenderTargetSize;

    // 2. Resize to training size (e.g., 128x128)
    TArray<FColor> ResizedBitmap;
    ResizedBitmap.SetNum(IMAGE_SIZE * IMAGE_SIZE);
    FImageUtils::ImageResize(Width, Height, Bitmap, IMAGE_SIZE, IMAGE_SIZE, ResizedBitmap, /*bLinear=*/true);

    // 3. Fill input tensor (channel-first) and normalize to [-1,1]
    for (int32 y = 0; y < IMAGE_SIZE; ++y)
    {
        for (int32 x = 0; x < IMAGE_SIZE; ++x)
        {
            int32 PixelIndex = y * IMAGE_SIZE + x;
            const FColor& Px = ResizedBitmap[PixelIndex];

            float r = Px.R / 255.f;
            float g = Px.G / 255.f;
            float b = Px.B / 255.f;

            // Normalize to [-1,1] like PyTorch
            r = (r - 0.5f) / 0.5f;
            g = (g - 0.5f) / 0.5f;
            b = (b - 0.5f) / 0.5f;

            // Channel-first layout: [C,H,W]
            ModelHelper->InputData[0 * IMAGE_SIZE * IMAGE_SIZE + y * IMAGE_SIZE + x] = r;
            ModelHelper->InputData[1 * IMAGE_SIZE * IMAGE_SIZE + y * IMAGE_SIZE + x] = g;
            ModelHelper->InputData[2 * IMAGE_SIZE * IMAGE_SIZE + y * IMAGE_SIZE + x] = b;
        }
    }

    return true;
}

FString AEnchantingStation::ClassifyRune()
{
    if (!ModelHelper.IsValid() || !ModelHelper->ModelInstance.IsValid())
    {
        UE_LOG(LogTemp, Warning, TEXT("Model not initialized"));
        return TEXT("None");
    }

    if (!PrepareInputTensor())
        return TEXT("Failed");

    if (ModelHelper->bIsRunning)
        return TEXT("Busy");

    ModelHelper->bIsRunning = true;

    static const TArray<FString> CLASSES = RuneClasses;

    AsyncTask(ENamedThreads::AnyNormalThreadNormalTask, [Helper = ModelHelper]()
        {
            // Run model synchronously
            Helper->ModelInstance->RunSync(Helper->InputBindings, Helper->OutputBindings);

            AsyncTask(ENamedThreads::GameThread, [Helper]()
                {
                    Helper->bIsRunning = false;

                    
                    int32 NumClasses = Helper->OutputData.Num();

                    // --- Apply softmax to get probabilities ---
                    TArray<float> Probabilities;
                    Probabilities.SetNumZeroed(NumClasses);

                    // 1. Find max logit for numerical stability
                    float MaxLogit = Helper->OutputData[0];
                    for (int32 i = 1; i < NumClasses; ++i)
                    {
                        if (Helper->OutputData[i] > MaxLogit)
                            MaxLogit = Helper->OutputData[i];
                    }

                    // 2. Compute exp(logit - max) and sum
                    float SumExp = 0.f;
                    for (int32 i = 0; i < NumClasses; ++i)
                    {
                        Probabilities[i] = FMath::Exp(Helper->OutputData[i] - MaxLogit);
                        SumExp += Probabilities[i];
                    }

                    // 3. Normalize
                    for (int32 i = 0; i < NumClasses; ++i)
                    {
                        Probabilities[i] /= SumExp;
                    }

                    // --- Print probabilities ---
                    TArray<FString> ProbStrings;
                    for (int32 i = 0; i < NumClasses; ++i)
                    {
                        FString ProbStr = FString::Printf(TEXT("%s: %.1f%%"), *CLASSES[i], Probabilities[i] * 100.f);
                        ProbStrings.Add(ProbStr);
                    }

                    UE_LOG(LogTemp, Log, TEXT("Rune classification probabilities:\n%s"), *FString::Join(ProbStrings, TEXT("\n")));

                    // --- Find max probability ---
                    int32 MaxIdx = 0;
                    float MaxVal = Probabilities[0];
                    for (int32 i = 1; i < NumClasses; ++i)
                    {
                        if (Probabilities[i] > MaxVal)
                        {
                            MaxVal = Probabilities[i];
                            MaxIdx = i;
                        }
                    }

                    FString DetectedRune = CLASSES.IsValidIndex(MaxIdx) ? CLASSES[MaxIdx] : TEXT("Unknown");
                    UE_LOG(LogTemp, Log, TEXT("Detected rune: %s"), *DetectedRune);
                });
        });

    return TEXT("Running...");
}




void AEnchantingStation::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!bIsDrawing || !CachedPC)
        return;

    FVector2D UV;
    if (!GetMouseUV(UV))
        return;

    if (!bHasLastUV)
    {
        CurrentStroke.Add(UV);
        LastUV = UV;
        bHasLastUV = true;
    }
    else
    {
        const int32 Steps = 8;
        for (int32 i = 1; i <= Steps; ++i)
        {
            FVector2D LerpUV = FMath::Lerp(LastUV, UV, i / (float)Steps);
            CurrentStroke.Add(LerpUV);
            DrawBrushAtUV(LerpUV);
        }
        LastUV = UV;
    }



}


void AEnchantingStation::StartDrawing()
{
    // Start a new stroke
    bIsDrawing = true;
    bHasLastUV = false;

    // Reset current stroke points, but DO NOT clear the render target!
    CurrentStroke.Reset();

    UE_LOG(LogTemp, Log, TEXT("Started new stroke"));
}


void AEnchantingStation::StopDrawing()
{
    bIsDrawing = false;
    if (CurrentStroke.Num() > 0)
    {
        RuneStrokes.Add(CurrentStroke);
        CurrentStroke.Reset();
    }
    bHasLastUV = false;
}

void AEnchantingStation::FinishRune()
{
    UE_LOG(LogTemp, Log, TEXT("Finished Rune with %d strokes"), RuneStrokes.Num());
    FString FileName = FString::Printf(TEXT("Rune_%d.png"), FDateTime::Now().GetTicks());

    if (EnchantingMode == EEnchantingMode::DatasetBuilder)
    {

        ExportRune(FileName);
    }
    else
    {
        FString rune = ClassifyRune();
        UE_LOG(LogTemp, Log, TEXT("Classified Rune as: %s"), *rune);
        ExportRune(FileName);
    }


    // Reset for next rune
    RuneStrokes.Reset();
    CurrentStroke.Reset();
    ClearRuneRenderTarget();
}


void AEnchantingStation::Exit_Implementation(ACharacter* Character)
{
    bIsDrawing = false;
    DrawPoints.Reset();
    bHasLastUV = false;

    UnbindInput();

    if (CachedPC)
    {
        CachedPC->bShowMouseCursor = false;
        CachedPC->SetInputMode(FInputModeGameOnly());
    }

    PrimaryActorTick.bCanEverTick = false;
    CachedPC = nullptr;

    if (EnchantingWidget)
    {
        EnchantingWidget->RemoveFromParent();
        EnchantingWidget = nullptr;
    }
}

void AEnchantingStation::CreateRuneRenderTarget()
{
    if (RuneRenderTarget) return;

    RuneRenderTarget = NewObject<UTextureRenderTarget2D>(this);
    RuneRenderTarget->RenderTargetFormat = RTF_RGBA8;
    RuneRenderTarget->ClearColor = FLinearColor(0, 0, 0, 1);
    RuneRenderTarget->InitAutoFormat(RenderTargetSize, RenderTargetSize);
    RuneRenderTarget->UpdateResourceImmediate(true);
}

void AEnchantingStation::ClearRuneRenderTarget()
{
    if (!RuneRenderTarget) return;

    UKismetRenderingLibrary::ClearRenderTarget2D(this, RuneRenderTarget, FLinearColor(0, 0, 0, 1));
}

void AEnchantingStation::CreateEnchantingWidget()
{
    if (!EnchantingWidgetClass || EnchantingWidget)
        return;

    EnchantingWidget = CreateWidget<UUserWidget>(GetWorld(), EnchantingWidgetClass);
    if (!EnchantingWidget)
        return;

    EnchantingWidget->AddToViewport(100);

    RuneImageWidget = Cast<UImage>(
        EnchantingWidget->GetWidgetFromName(TEXT("RuneImage"))
    );

    if (RuneImageWidget && RuneRenderTargetMID)
    {
        RuneImageWidget->SetBrushFromMaterial(RuneRenderTargetMID);
    }
}


void AEnchantingStation::DrawBrushAtUV(const FVector2D& UV)
{
    if (!RuneRenderTarget || !RuneBrushMID)
        return;

    // Flip Y ONCE for render target space
    RuneBrushMID->SetVectorParameterValue(
        TEXT("BrushCenter"),
        FLinearColor(UV.X, UV.Y, 0.f)
    );

    UKismetRenderingLibrary::DrawMaterialToRenderTarget(
        this,
        RuneRenderTarget,
        RuneBrushMID
    );
}



bool AEnchantingStation::GetMouseUV(FVector2D& OutUV) const
{
    if (!CachedPC || !RuneImageWidget)
        return false;

    // 1. Get absolute mouse position in Slate space
    const FVector2D AbsoluteMousePos =
        FSlateApplication::Get().GetCursorPos();


    // 2. Widget geometry 
    const FGeometry& Geo = RuneImageWidget->GetCachedGeometry();

    // 3. Convert absolute -> local widget space
    const FVector2D LocalPos = Geo.AbsoluteToLocal(AbsoluteMousePos);
    const FVector2D LocalSize = Geo.GetLocalSize();

    if (LocalSize.X <= 0.f || LocalSize.Y <= 0.f)
        return false;

    // 4. Reject if outside draw area
    if (LocalPos.X < 0.f || LocalPos.Y < 0.f ||
        LocalPos.X > LocalSize.X || LocalPos.Y > LocalSize.Y)
    {
        return false;
    }

    // 5. Normalize to UVs
    OutUV.X = LocalPos.X / LocalSize.X;
    OutUV.Y = 1.f - (LocalPos.Y / LocalSize.Y); // flip Y for RT

	UE_LOG(LogTemp, Log, TEXT("Mouse UV: %s"), *OutUV.ToString());

    return true;
}




void AEnchantingStation::BindInput(APlayerController* PC)
{
    if (!PC) return;

    EnableInput(PC);

    CachedEnhancedInput = Cast<UEnhancedInputComponent>(InputComponent);
    if (!CachedEnhancedInput)
    {
        UE_LOG(LogTemp, Error, TEXT("No EnhancedInputComponent"));
        return;
    }

    CachedEnhancedInput->BindAction(DrawRuneAction, ETriggerEvent::Started, this, &AEnchantingStation::StartDrawing);
    CachedEnhancedInput->BindAction(DrawRuneAction, ETriggerEvent::Completed, this, &AEnchantingStation::StopDrawing);
    CachedEnhancedInput->BindAction(FinishRuneAction, ETriggerEvent::Started, this, &AEnchantingStation::FinishRune);

}

void AEnchantingStation::UnbindInput()
{
    if (!CachedEnhancedInput) return;
    CachedEnhancedInput->ClearActionBindings();
    CachedEnhancedInput = nullptr;
    DisableInput(nullptr);
}

void AEnchantingStation::ExportRune(const FString& FileName)
{
    if (!RuneRenderTarget) return;

    FTextureRenderTargetResource* RTResource = RuneRenderTarget->GameThread_GetRenderTargetResource();
    if (!RTResource) return;

    TArray<FColor> Bitmap;
    if (!RTResource->ReadPixels(Bitmap))
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to read render target pixels"));
        return;
    }

    int32 Width = RuneRenderTarget->SizeX;
    int32 Height = RuneRenderTarget->SizeY;

    // Convert to black-and-white
    TArray<FColor> BinaryBitmap;
    BinaryBitmap.Reserve(Bitmap.Num());

    for (const FColor& Pixel : Bitmap)
    {
        uint8 Gray = FMath::RoundToInt(0.299f * Pixel.R + 0.587f * Pixel.G + 0.114f * Pixel.B);
        uint8 BW = (Gray > 10) ? 255 : 0;
        BinaryBitmap.Add(FColor(BW, BW, BW, 255));
    }

    // Encode as PNG
    IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
    TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG);

    ImageWrapper->SetRaw(BinaryBitmap.GetData(), BinaryBitmap.GetAllocatedSize(), Width, Height, ERGBFormat::RGBA, 8);

    const FString SavePath = FPaths::ProjectDir() / "RuneDataset" / FileName;
    IFileManager::Get().MakeDirectory(*FPaths::GetPath(SavePath), true);

    TArray<uint8, FDefaultAllocator64> PNGData = ImageWrapper->GetCompressed();
    if (FFileHelper::SaveArrayToFile(PNGData, *SavePath))
    {
        UE_LOG(LogTemp, Log, TEXT("Saved rune to %s"), *SavePath);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to save rune PNG"));
    }
}

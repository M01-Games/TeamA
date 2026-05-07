#pragma once

#include "CoreMinimal.h"
#include "Pickup.h"
#include "ItemTypesEnum.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Project.generated.h"

/**
 *
 */
	UCLASS()
	class TEAMA_API AProject : public APickup
{
	GENERATED_BODY()

public:
	AProject();

	//Skeletal mesh component
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMeshComponent* SkeletalMesh;

	//Variables for Forging minigame
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsForged;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<float> ForgingPattern;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float forgingProgress;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TotalForgeScore;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 TotalForgeHits;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float FinalForgeScore;

	void FinalizeForgingScore();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float GrainStrength = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HeatIntensity;

	bool bIsInHeat = false;

	UFUNCTION(BlueprintNativeEvent)
	void ForgeModel();

	//Array of strings representing the inscribed runes
	UPROPERTY(EditAnywhere, BlueprintReadWrite) 
	TArray<FString> InscribedRunes;

	bool bIsEnchanted = false;

	//Arrow component for TipSide and HandleSide indicators
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* TipSide;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* HandleSide;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EEnchants Enchant;
	
protected:
	// On overlap stay event
	void NotifyActorBeginOverlap(AActor* OtherActor) override;
	void NotifyActorEndOverlap(AActor* OtherActor) override;

	// tick
	virtual void Tick(float DeltaTime) override;
};

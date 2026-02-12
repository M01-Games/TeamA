#pragma once

#include "CoreMinimal.h"
#include "Pickup.h"
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
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bIsForged;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<float> ForgingPattern;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float forgingProgress;
	float TotalForgeScore;
	int32 TotalForgeHits;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float FinalForgeScore;

	void FinalizeForgingScore();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float GrainStrength = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HeatIntensity;

	bool bIsInHeat = false;

	UFUNCTION(BlueprintNativeEvent)
	void ForgeModel();



protected:
	// On overlap stay event
	void NotifyActorBeginOverlap(AActor* OtherActor) override;
	void NotifyActorEndOverlap(AActor* OtherActor) override;

	// tick
	virtual void Tick(float DeltaTime) override;
};

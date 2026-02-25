#include "Project.h"
#include "ItemTypesEnum.h"

AProject::AProject()
{
	ItemType = EItemTypesEnum::Project;


	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	//CollisionMesh->SetVisibility(false);

	// Attach SkeletalMesh to CollisionMesh
	SkeletalMesh->SetupAttachment(InteractionVolume);

	// Setup Arrow Components
	
	TipSide = CreateDefaultSubobject<USceneComponent>(TEXT("TipSide"));
	HandleSide = CreateDefaultSubobject<USceneComponent>(TEXT("HandleSide"));
	TipSide->SetupAttachment(Root);
	HandleSide->SetupAttachment(Root);
	
	// Make sure the arrow components are editable and visible in the editor


	//Set default values for forging minigame
	bIsForged = false;
	forgingProgress = 0.0f;
	ForgingPattern = { 0.3, 0.3, 0.9 }; // Example pattern
}

void AProject::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	if (bIsInHeat) {
		HeatIntensity += DeltaTime * 10.0f; // Increase heat over time when in heat source 
		HeatIntensity = FMath::Clamp(HeatIntensity, 0.0f, 100.0f); }
	else {
		HeatIntensity -= DeltaTime * 3.0f; // Cool down when not in heat source 
		HeatIntensity = FMath::Clamp(HeatIntensity, 0.0f, 100.0f);
	}
}

void AProject::FinalizeForgingScore()
{
	if (TotalForgeHits <= 0)
	{
		FinalForgeScore = 0.0f;
		return;
	}

	// Average hit score 0–1
	float AverageScore = TotalForgeScore / TotalForgeHits;

	// Convert to percentage
	FinalForgeScore = FMath::Clamp(AverageScore * 100.0f, 0.0f, 100.0f);
}

void AProject::ForgeModel_Implementation()
{

}

void AProject::NotifyActorBeginOverlap(AActor* OtherActor) {
	Super::NotifyActorBeginOverlap(OtherActor); 
	// If the other actor has the Heat tag
	if (OtherActor->ActorHasTag("Heat"))
	{
		bIsInHeat = true;
	}
} 

void AProject::NotifyActorEndOverlap(AActor* OtherActor) {
	Super::NotifyActorEndOverlap(OtherActor);
	if (OtherActor->ActorHasTag("Heat")) 
	{ 
		bIsInHeat = false;
	}
}
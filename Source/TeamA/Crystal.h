// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickup.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Project.h"
#include "Crystal.generated.h"

/**
 * 
 */
UCLASS()
class TEAMA_API ACrystal : public APickup
{
	GENERATED_BODY()
	
public:
	ACrystal();

	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//UGeometryCollectionComponent* CrystalMesh;

	UFUNCTION(BlueprintCallable)
	void Shatter();


	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindComponent))
	UStaticMeshComponent* StaticMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UGeometryCollectionComponent* GeometryCollection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* FieldActor; // Actor that applies forces to the shattering pieces

protected:
	void BeginPlay() override;
};

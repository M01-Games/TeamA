// Fill out your copyright notice in the Description page of Project Settings.

//Include geometry collection for collision and interaction

#include "Crystal.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "ItemSlot.h"
#include "GeometryCollection/GeometryCollectionComponent.h"

// Sets default values
ACrystal::ACrystal()
{

}

void ACrystal::Shatter()
{
	//Hide the static mesh and show the geometry collection to create a shattering effect
	if (StaticMesh)
		{
		StaticMesh->SetVisibility(false);
	}
	if (GeometryCollection)
	{
		GeometryCollection->SetVisibility(true);
		GeometryCollection->SetSimulatePhysics(true);
		GeometryCollection->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	}

	

	//Add an impulse to the geometry collection to make it shatter
	
	if (GeometryCollection)
	{
		FVector ImpulseDirection = FVector(0, 0, 1); // Upward impulse
		float ImpulseStrength = 150.0f;
		GeometryCollection->AddImpulse(ImpulseDirection * ImpulseStrength, NAME_None, true);
	}

	// get actors we are attached to
	AActor* AttachedActor = GetAttachParentActor();
	if (AttachedActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("Crystal is attached to: %s"), *AttachedActor->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Crystal is not attached to any actor."));
	}
	//if attached actor is an item slot, call the item slots DetachItem function to detach the crystal from the slot
	if (AttachedActor)
	{
		AItemSlot* ItemSlot = Cast<AItemSlot>(AttachedActor);
		if (ItemSlot)
		{
			UE_LOG(LogTemp, Warning, TEXT("Crystal is attached to an ItemSlot: %s"), *ItemSlot->GetName());
			ItemSlot->DetachItem();
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Attached actor is not an ItemSlot."));
		}
	}

	//Kill the crystal actor after a short delay to allow the shattering effect to play out
	SetLifeSpan(5.0f);


}

void ACrystal::BeginPlay()
{
	Super::BeginPlay();
	
	StaticMesh = FindComponentByClass<UStaticMeshComponent>();
	GeometryCollection = FindComponentByClass<UGeometryCollectionComponent>();

	UE_LOG(LogTemp, Warning, TEXT("Crystal %s initialized with StaticMesh: %s and GeometryCollection: %s"), *GetName(), StaticMesh ? *StaticMesh->GetName() : TEXT("None"), GeometryCollection ? *GeometryCollection->GetName() : TEXT("None"));
}
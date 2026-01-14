// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickup.h"

// Sets default values
APickup::APickup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	// Create mesh component
	CollisionMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CollisionMesh"));

	// simulate physics
	CollisionMesh->SetSimulatePhysics(true);
	// Generate overlap events
	CollisionMesh->SetGenerateOverlapEvents(true);
	// Enable collision
	CollisionMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	RootComponent = CollisionMesh;
}

// Called when the game starts or when spawned
void APickup::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APickup::OnPickedUp()
{
	// You can add custom logic here for when the item is picked up
	// For example, play a sound or spawn a particle effect
}

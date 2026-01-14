#include "ItemSlot.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "DrawDebugHelpers.h"
#include "Workstation.h"
#include "Pickup.h"

AItemSlot::AItemSlot()
{
    PrimaryActorTick.bCanEverTick = false;

    SocketPoint = CreateDefaultSubobject<USceneComponent>(TEXT("SocketPoint"));
    RootComponent = SocketPoint;

    SocketMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SocketMesh"));
    SocketMesh->SetupAttachment(RootComponent);
    SocketMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    bIsOccupied = false;
    AttachedItem = nullptr;

    AcceptedItemType = EItemTypesEnum::Project; // Default allows any type

    InteractionVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionVolume"));
    InteractionVolume->SetupAttachment(RootComponent);
    InteractionVolume->SetCollisionProfileName(TEXT("BlockAll"));
    InteractionVolume->SetGenerateOverlapEvents(true);
    DrawDebugBox(GetWorld(), SocketPoint->GetComponentLocation(), FVector(20.f), FColor::Green, false, 2.f);


}

bool AItemSlot::AttachItem(APickup* Item)
{
    if (!Item || bIsOccupied)
        return false;

    // Type check
    if (Item->ItemType != AcceptedItemType)
    {
        return false;
    }

    // Attach item to socket
    Item->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
    Item->CollisionMesh->SetSimulatePhysics(false);
    Item->CollisionMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    Item->AttachToComponent(
        SocketPoint,
        FAttachmentTransformRules::SnapToTargetNotIncludingScale
    );

    Item->SetActorRelativeLocation(FVector::ZeroVector);
    Item->SetActorRelativeRotation(FRotator::ZeroRotator);

    AttachedItem = Item;
    bIsOccupied = true;

	//Check if parent actor is a workstation
	AActor* ParentActor = GetAttachParentActor();
	if (ParentActor)
	{
		//If it is, notify the workstation that an item has been attached
		AWorkstation* Workstation = Cast<AWorkstation>(ParentActor);
		if (Workstation)
		{
			Workstation->Inventory.Add(Item);
		}
	}


    return true;
}

void AItemSlot::DetachItem()
{
    if (!AttachedItem)
        return;

    AttachedItem->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
    AttachedItem->CollisionMesh->SetSimulatePhysics(true);
    AttachedItem->CollisionMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

    AttachedItem = nullptr;
    bIsOccupied = false;
}

APickup* AItemSlot::TakeItem()
{
    if (!AttachedItem)
        return nullptr;

    APickup* Item = AttachedItem;

    AttachedItem = nullptr;
    bIsOccupied = false;

	//Check if parent actor is a workstation
	AActor* ParentActor = GetAttachParentActor();
	if (ParentActor)
	{
		//If it is, notify the workstation that an item has been removed
		AWorkstation* Workstation = Cast<AWorkstation>(ParentActor);
		if (Workstation)
		{
			Workstation->Inventory.Remove(Item);
		}
	}

    return Item;
}

#include "ItemTypesEnum.h"
#include "Project.h"

AProject::AProject()
{
	ItemType = EItemTypesEnum::Project;


	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	CollisionMesh->SetVisibility(false);

	// Attach SkeletalMesh to CollisionMesh
	SkeletalMesh->SetupAttachment(RootComponent);
}
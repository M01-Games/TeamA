#pragma once

#include "CoreMinimal.h"
#include "ItemTypesEnum.generated.h"

/**
 * Custom enum to demonstrate how to expose enums to Blueprints.
 */
UENUM(BlueprintType)
enum class EItemTypesEnum : uint8
{
    Ore UMETA(DisplayName = "Ore"),
    Project UMETA(DisplayName = "Project"),
    Money UMETA(DisplayName = "Money"),
	Handle UMETA(DisplayName = "Handle"),
	MOULD UMETA(DisplayName = "Mould")
};

UENUM(BlueprintType)
enum class ESpecificTypes : uint8
{
    // Ore subtypes
    Iron UMETA(DisplayName = "Iron"),
    Copper UMETA(DisplayName = "Copper"),
    Silver UMETA(DisplayName = "Silver"),
    Gold UMETA(DisplayName = "Gold"),
	Tungsten UMETA(DisplayName = "Tungsten"),
	Zinc UMETA(DisplayName = "Zinc"),
    // Project subtypes
    Sword UMETA(DisplayName = "Sword"),
	Dagger UMETA(DisplayName = "Dagger"),
	Axe UMETA(DisplayName = "Axe"),
	None UMETA(DisplayName = "None")
};
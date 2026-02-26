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
	MOULD UMETA(DisplayName = "Mould"),
	CRYSTAL UMETA(DisplayName = "Crystal")
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

UENUM(BlueprintType)
enum class EEnchants : uint8
{
    Fire UMETA(DisplayName = "Fire"),
    Ice UMETA(DisplayName = "Ice"),
    Lightning UMETA(DisplayName = "Lightning"),
	Luck UMETA(DisplayName = "Luck"),
	Poison UMETA(DisplayName = "Poison"),
	Light UMETA(DisplayName = "Light"),
	Dark UMETA(DisplayName = "Dark"),
	Air UMETA(DisplayName = "Air"),
	Magic UMETA(DisplayName = "Magic"),
    None UMETA(DisplayName = "None")
};
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "TeamACharacter.h"
#include "SaveGame_Blacksmith.generated.h"

/**
 * 
 */
UCLASS()
class TEAMA_API USaveGame_Blacksmith : public USaveGame
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SaveData")

};

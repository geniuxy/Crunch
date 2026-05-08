// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "NetFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class CRUNCH_API UNetFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static uint8 GetPlayerCountPerTeam();
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CStormCore.generated.h"

UCLASS()
class CRUNCH_API ACStormCore : public ACharacter
{
	GENERATED_BODY()

public:
	ACStormCore();

protected:
	virtual void BeginPlay() override;
};

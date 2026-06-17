// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "UObject/Object.h"
#include "MMC_CostValue.generated.h"

/**
 * 
 */
UCLASS()
class CRUNCH_API UMMC_CostValue : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()

public:
	UMMC_CostValue();

	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;

private:
	FGameplayEffectAttributeCaptureDefinition CostReductionDef;
};

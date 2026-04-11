// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/ModMagnitudeCalculations/MMC_LevelBased.h"

#include "AbilitySystemComponent.h"

UMMC_LevelBased::UMMC_LevelBased()
{
	LevelCaptureDef.AttributeToCapture = RateAttribute;
	LevelCaptureDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;

	RelevantAttributesToCapture.Add(LevelCaptureDef);
}

float UMMC_LevelBased::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	UAbilitySystemComponent* ASC = Spec.GetContext().GetInstigatorAbilitySystemComponent();
	if (!ASC) return 0.f;

	float Level = 0.f;
	FAggregatorEvaluateParameters EvalParams;
	EvalParams.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	EvalParams.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	bool bFound;
	float RateAttributeVal = ASC->GetGameplayAttributeValue(RateAttribute, bFound);
	if (!bFound) return 0.f;

	return (Level - 1) * RateAttributeVal;
}

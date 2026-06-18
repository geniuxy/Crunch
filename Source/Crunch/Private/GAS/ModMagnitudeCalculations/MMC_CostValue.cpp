// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/ModMagnitudeCalculations/MMC_CostValue.h"

#include "CGameplayTags.h"
#include "GAS/CAttributeSet.h"

UMMC_CostValue::UMMC_CostValue()
{
	CostReductionDef.AttributeToCapture = UCAttributeSet::GetCostReductionAttribute();
	CostReductionDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Source;

	RelevantAttributesToCapture.Add(CostReductionDef);
}

float UMMC_CostValue::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	FAggregatorEvaluateParameters EvalParams;
	EvalParams.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	EvalParams.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	float CostReduction = 0.f;
	GetCapturedAttributeMagnitude(CostReductionDef, Spec, EvalParams, CostReduction);

	float BaseCost = Spec.GetSetByCallerMagnitude(CGameplayTags::Crunch_SetByCaller_BaseCost, false, 0.0f);

	float EffectiveCost = BaseCost * (1.0f - FMath::Clamp(CostReduction, 0.f, 1.0f));
        
	return FMath::Min(0.0f, EffectiveCost);
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/ModMagnitudeCalculations/MMC_CooldownDuration.h"

#include "CGameplayTags.h"
#include "GAS/CAttributeSet.h"

UMMC_CooldownDuration::UMMC_CooldownDuration()
{
	CooldownReductionDef.AttributeToCapture = UCAttributeSet::GetCooldownReductionAttribute();
	CooldownReductionDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Source;

	RelevantAttributesToCapture.Add(CooldownReductionDef);
}

float UMMC_CooldownDuration::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	FAggregatorEvaluateParameters EvalParams;
	EvalParams.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	EvalParams.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	float CooldownReduction = 0.f;
	GetCapturedAttributeMagnitude(CooldownReductionDef, Spec, EvalParams, CooldownReduction);

	float BaseCooldown = Spec.GetSetByCallerMagnitude(CGameplayTags::Crunch_SetByCaller_BaseCooldown, false, 0.0f);

	float EffectiveCooldown = BaseCooldown * (1.0f - FMath::Clamp(CooldownReduction, -1.0f, 1.0f));
        
	return FMath::Max(0.0f, EffectiveCooldown);
}

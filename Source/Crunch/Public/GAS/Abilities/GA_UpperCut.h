// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CGameplayAbilityBase.h"
#include "GA_UpperCut.generated.h"

/**
 * 
 */
UCLASS()
class CRUNCH_API UGA_UpperCut : public UCGameplayAbilityBase
{
	GENERATED_BODY()

public:
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;

private:
	UPROPERTY(EditDefaultsOnly, Category="Targeting")
	float TargetSweepSphereRadius = 120.f;
	
	UPROPERTY(EditDefaultsOnly, Category="Animation")
	UAnimMontage* UpperCutMontage;

	UFUNCTION()
	void StartLaunching(FGameplayEventData EventData);
};

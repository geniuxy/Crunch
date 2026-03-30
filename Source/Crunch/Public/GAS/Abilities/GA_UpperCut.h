// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CGameplayAbilityBase.h"
#include "CTypes/CStruct.h"
#include "GA_UpperCut.generated.h"

/**
 * 
 */
UCLASS()
class CRUNCH_API UGA_UpperCut : public UCGameplayAbilityBase
{
	GENERATED_BODY()

public:
	UGA_UpperCut();
	
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;

private:
	UPROPERTY(EditDefaultsOnly, Category="Damage")
	TMap<FName, FGenericDamageEffectDef> ComboDamageMap;
	
	UPROPERTY(EditDefaultsOnly, Category="Launch")
	TSubclassOf<UGameplayEffect> LaunchDamageEffect;
	
	UPROPERTY(EditDefaultsOnly, Category="Launch")
	float UpperCutLaunchSpeed = 1000.f;

	UPROPERTY(EditDefaultsOnly, Category="Launch")
	float UpperCutHoldSpeed = 100.f;
	
	UPROPERTY(EditDefaultsOnly, Category="Targeting")
	float TargetSweepSphereRadius = 120.f;
	
	UPROPERTY(EditDefaultsOnly, Category="Animation")
	UAnimMontage* UpperCutMontage;

	const FGenericDamageEffectDef* GetDamageEffectDefForCurrentCombo() const;

	UFUNCTION()
	void StartLaunching(FGameplayEventData EventData);

	UFUNCTION()
	void HandleComboChangeEvent(FGameplayEventData EventData);
	UFUNCTION()
	void HandleBasicAttackComboCommitEvent(FGameplayEventData EventData);
	UFUNCTION()
	void HandleBasicAttackComboDamageEvent(FGameplayEventData EventData);

	FName NextComboName;
};

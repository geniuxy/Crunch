// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CGameplayAbilityBase.h"
#include "GA_Combo.generated.h"

/**
 * 
 */
UCLASS()
class CRUNCH_API UGA_Combo : public UCGameplayAbilityBase
{
	GENERATED_BODY()

public:
	UGA_Combo();

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;

private:
	/*
	 * Combo
	 */
	void SetupWaitComboInputPress();
	
	UFUNCTION()
	void HandleInputPress(float TimeWaited);
	
	void TryCommitCombo();

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* ComboMontage;

	FName NextComboName;
	
	UFUNCTION()
	void ComboChangedEventReceived(FGameplayEventData Data);
	
	/*
	 * Damage
	 */
	UPROPERTY(EditAnywhere, Category="Gameplay Effect")
	float TargetSweepSphereRadius = 30.f;
	
	UPROPERTY(EditAnywhere, Category="Gameplay Effect")
	TSubclassOf<UGameplayEffect> DefaultGameplayEffect;

	UPROPERTY(EditAnywhere, Category="Gameplay Effect")
	TMap<FName, TSubclassOf<UGameplayEffect>> DamageEffectMap;

	UFUNCTION()
	void DoDamage(FGameplayEventData Data);

	TSubclassOf<UGameplayEffect> GetCurrentEffectForCurrentCombo() const;
	
};

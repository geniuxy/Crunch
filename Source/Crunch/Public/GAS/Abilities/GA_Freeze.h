// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CGameplayAbilityBase.h"
#include "GA_Freeze.generated.h"

class ATargetActor_GroundPick;
/**
 * 
 */
UCLASS()
class CRUNCH_API UGA_Freeze : public UCGameplayAbilityBase
{
	GENERATED_BODY()

public:
	UGA_Freeze();

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;

private:
	UPROPERTY(EditDefaultsOnly, Category="Targeting")
	float TargetingAreaRadius = 1000.f;
	
	UPROPERTY(EditDefaultsOnly, Category="Targeting")
	float TargetRange = 2000.f;

	UPROPERTY(EditDefaultsOnly, Category="Targeting")
	TSubclassOf<ATargetActor_GroundPick> TargetActorClass;

	UPROPERTY(EditDefaultsOnly, Category="Animation")
	UAnimMontage* FreezeTargetingMontage;

	UPROPERTY(EditDefaultsOnly, Category="Animation")
	UAnimMontage* FreezeCastMontage;

	UPROPERTY(EditDefaultsOnly, Category="Effect")
	TSubclassOf<UGameplayEffect> DamageEffect;
	
	UPROPERTY(EditDefaultsOnly, Category="GameplayCue", meta=(Categories="GameplayCue"))
	FGameplayTag FreezingImpactGameplayCueTag;
	
	UFUNCTION()
	void TargetReceived(const FGameplayAbilityTargetDataHandle& TargetDataHandle);

	UFUNCTION()
	void TargetCancelled(const FGameplayAbilityTargetDataHandle& TargetDataHandle);
};

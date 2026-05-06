// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CGameplayAbilityBase.h"
#include "GA_BlackHole.generated.h"

class ATargetActor_GroundPick;
/**
 * 
 */
UCLASS()
class CRUNCH_API UGA_BlackHole : public UCGameplayAbilityBase
{
	GENERATED_BODY()

public:
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;

	virtual void EndAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled
	) override;

private:
	UPROPERTY(EditDefaultsOnly, Category="Targeting")
	float TargetAreaRadius = 300.f;
	
	UPROPERTY(EditDefaultsOnly, Category="Targeting")
	float TargetTraceRange = 2000.f;

	UPROPERTY(EditDefaultsOnly, Category="Targeting")
	TSubclassOf<ATargetActor_GroundPick> TargetActorClass;
	
	UPROPERTY(EditDefaultsOnly, Category="Animation")
	UAnimMontage* TargetingMontage;

	UPROPERTY(EditDefaultsOnly, Category="Effects")
	TSubclassOf<UGameplayEffect> AimEffect;

	FActiveGameplayEffectHandle AimEffectHandle;

	void AddAimEffect();
	void RemoveAimEffect();

	UFUNCTION()
	void PlaceBlackHole(const FGameplayAbilityTargetDataHandle& TargetDataHandle);

	UFUNCTION()
	void PlacementCancelled(const FGameplayAbilityTargetDataHandle& TargetDataHandle);
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CGameplayAbilityBase.h"
#include "GA_Tornado.generated.h"

/**
 * 
 */
UCLASS()
class CRUNCH_API UGA_Tornado : public UCGameplayAbilityBase
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
	UPROPERTY(EditDefaultsOnly, Category="Effect")
	TSubclassOf<UGameplayEffect> HitDamageEffect;

	UPROPERTY(EditDefaultsOnly, Category="Anim")
	UAnimMontage* TornadoMontage;
	
	UPROPERTY(EditDefaultsOnly, Category="Ability Details")
	float HitPushSpeed = 3000.f;

	UPROPERTY(EditDefaultsOnly, Category="Anim")
	float TornadoDuration = 4.f;

	UFUNCTION()
	void TornadoDamageEffectReceived(FGameplayEventData Payload);
};

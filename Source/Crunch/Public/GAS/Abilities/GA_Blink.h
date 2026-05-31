// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CGameplayAbilityBase.h"
#include "GA_Blink.generated.h"

class ATargetActor_GroundPick;
/**
 * 
 */
UCLASS()
class CRUNCH_API UGA_Blink : public UCGameplayAbilityBase
{
	GENERATED_BODY()

public:
	UGA_Blink();
	
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;

private:
	UPROPERTY(EditDefaultsOnly, Category="Anim")
	UAnimMontage* TargetingMontage;
	
	UPROPERTY(EditDefaultsOnly, Category="Anim")
	UAnimMontage* TeleportMontage;

	UPROPERTY(EditDefaultsOnly, Category="Targeting")
	TSubclassOf<ATargetActor_GroundPick> GroundPickTargetActorClass;

	UPROPERTY(EditDefaultsOnly, Category="Targeting")
	float TargetAreaRadius = 1000.f;

	UPROPERTY(EditDefaultsOnly, Category="Effect")
	TSubclassOf<UGameplayEffect> DamageEffect;

	UPROPERTY(EditDefaultsOnly, Category="Ability Details")
	float BlinkCastRange = 3000.f;

	UPROPERTY(EditDefaultsOnly, Category="Ability Details")
	float BlinkLandTargetPushSpeed = 5000.f;

	UFUNCTION()
	void GroundPickTargetReceived(const FGameplayAbilityTargetDataHandle& TargetDataHandle);

	UFUNCTION()
	void GroundPickCancelled(const FGameplayAbilityTargetDataHandle& TargetDataHandle);

	UFUNCTION()
	void Teleport(FGameplayEventData Payload);

	FGameplayAbilityTargetDataHandle BlinkTargetDataHandle;
};

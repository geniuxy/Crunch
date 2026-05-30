// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CGameplayAbilityBase.h"
#include "GA_Dash.generated.h"

class ATargetActor_Around;
class UCharacterMovementComponent;
/**
 * 
 */
UCLASS()
class CRUNCH_API UGA_Dash : public UCGameplayAbilityBase
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
	UPROPERTY(EditDefaultsOnly, Category="Anim")
	UAnimMontage* DashMontage;

	UPROPERTY(EditDefaultsOnly, Category="Targeting")
	float TargetDetectionRadius = 300.f;

	UPROPERTY(EditDefaultsOnly, Category="Targeting")
	FName TargetActorAttachSocketName = "TargetDashCenter";

	UPROPERTY(EditDefaultsOnly, Category="Targeting")
	TSubclassOf<ATargetActor_Around> TargetActorClass;
	
	UPROPERTY(EditDefaultsOnly, Category="Gameplay Cue")
	FGameplayTag LocalGameplayCueTag;

	UPROPERTY(EditDefaultsOnly, Category="Ability Details")
	float TargetHitPushSpeed = 3000.f;

	UPROPERTY(EditDefaultsOnly, Category="Effects")
	TSubclassOf<UGameplayEffect> DamageEffect;

	UPROPERTY(EditDefaultsOnly, Category="Effects")
	TSubclassOf<UGameplayEffect> DashEffect;

	FActiveGameplayEffectHandle DashEffectHandle;

	FTimerHandle PushForwardInputTimerHandle;

	float CachedSpeedAcceleration = 0;

	void PushForward();

	UPROPERTY()
	UCharacterMovementComponent* OwnerCharacterMovementComponent;

	UFUNCTION()
	void StartDash(FGameplayEventData Payload);

	UFUNCTION()
	void TargetReceived(const FGameplayAbilityTargetDataHandle& TargetDataHandle);
};

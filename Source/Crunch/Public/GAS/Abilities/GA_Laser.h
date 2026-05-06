// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CGameplayAbilityBase.h"
#include "GA_Laser.generated.h"

class ATargetActor_Line;
/**
 * 
 */
UCLASS()
class CRUNCH_API UGA_Laser : public UCGameplayAbilityBase
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
	UPROPERTY(EditDefaultsOnly, Category="Effects")
	TSubclassOf<UGameplayEffect> OnGoingConsumptionEffect;

	UPROPERTY(EditDefaultsOnly, Category="Effects")
	TSubclassOf<UGameplayEffect> HitDamageEffect;

	UPROPERTY(EditDefaultsOnly, Category="Effects")
	float HitPushSpeed = 3000.f;

	FActiveGameplayEffectHandle OnGoingConsumptionEffectHandle;

	UPROPERTY(EditDefaultsOnly, Category="Anim")
	UAnimMontage* LaserMontage;

	UPROPERTY(EditDefaultsOnly, Category="Targeting")
	FName TargetActorAttachSocketName = "Laser";
	
	UPROPERTY(EditDefaultsOnly, Category="Targeting")
	TSubclassOf<ATargetActor_Line> LaserTargetActorClass;

	UPROPERTY(EditDefaultsOnly, Category="Targeting")
	float TargetRange = 4000.f;
	
	UPROPERTY(EditDefaultsOnly, Category="Targeting")
	float DetectionCylinderRadius = 30.f;

	UPROPERTY(EditDefaultsOnly, Category="Targeting")
	float TargetingInterval = 0.3f;

	UFUNCTION()
	void ShootLaser(FGameplayEventData Payload);

	void ManaUpdated(const FOnAttributeChangeData& ChangeData);

	UFUNCTION()
	void TargetActorReceived(const FGameplayAbilityTargetDataHandle& TargetDataHandle);
};

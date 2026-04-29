// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "Abilities/GameplayAbility.h"
#include "CGameplayAbilityBase.generated.h"

/**
 * 
 */
UCLASS()
class CRUNCH_API UCGameplayAbilityBase : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UCGameplayAbilityBase();

	virtual bool CanActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayTagContainer* SourceTags = nullptr,
		const FGameplayTagContainer* TargetTags = nullptr,
		FGameplayTagContainer* OptionalRelevantTags = nullptr
	) const override;

protected:
	UAnimInstance* GetOwnerAnimInstance() const;

	TArray<FHitResult> GetHitResultFromSweepLocationTargetData(
		const FGameplayAbilityTargetDataHandle& TargetDataHandle,
		float SphereSweepRadius = 30.f,
		ETeamAttitude::Type TargetTeam = ETeamAttitude::Hostile, // 目标Team属性
		bool bDrawDebug = false,
		bool bIgnoreSelf = true
	) const;

	UPROPERTY(EditDefaultsOnly, Category="debug")
	bool bShouldDrawDebug = false;

	void PushSelf(const FVector& PushVel);
	void PushTarget(AActor* Target, const FVector& PushVel);
	void PushTargets(const TArray<AActor*>& Targets, const FVector& PushVel);
	void PushTargets(const FGameplayAbilityTargetDataHandle& TargetDataHandle, const FVector& PushVel);

	void PlayMontageLocally(UAnimMontage* MontageToPlay);
	void StopMontageAfterCurrentSection(UAnimMontage* MontageToStop);

	ACharacter* GetOwnerAvatarCharacter();

	UPROPERTY()
	ACharacter* OwnerAvatarCharacter;

	void ApplyGameplayEffectToHitResultActor(
		const FHitResult& HitResult, TSubclassOf<UGameplayEffect> GameplayEffect, int Level = 1
	);

public:
	FORCEINLINE bool ShouldDrawDebug() const { return bShouldDrawDebug; }
};

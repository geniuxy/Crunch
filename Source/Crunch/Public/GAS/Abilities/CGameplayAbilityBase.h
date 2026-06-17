// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "Abilities/GameplayAbility.h"
#include "CGameplayAbilityBase.generated.h"

struct FAbilityData;
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

	virtual const FGameplayTagContainer* GetCooldownTags() const override;

	virtual bool CheckCooldown(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		FGameplayTagContainer* OptionalRelevantTags = nullptr
	) const override;

	virtual bool CheckCost(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		FGameplayTagContainer* OptionalRelevantTags = nullptr
	) const override;

	virtual void ApplyCooldown(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo
	) const override;

	virtual void ApplyCost(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo
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
	void PushTargetsFromOwnerLocation(const TArray<AActor*>& Targets, float PushSpeed);
	void PushTargetsFromOwnerLocation(const FGameplayAbilityTargetDataHandle& TargetDataHandle, float PushSpeed);
	void PushTargetsFromLocation(
		const FGameplayAbilityTargetDataHandle& TargetDataHandle, const FVector& FromLocation, float PushSpeed
	);
	void PushTargetsFromLocation(const TArray<AActor*>& Targets, const FVector& FromLocation, float PushSpeed);

	void PlayMontageLocally(UAnimMontage* MontageToPlay);
	void StopMontageAfterCurrentSection(UAnimMontage* MontageToStop);

	FGenericTeamId GetOwnerTeamID() const;

	bool IsOtherActorTeamAttitudeIs(const AActor* OtherActor, ETeamAttitude::Type TargetTeamAttitude) const;

	ACharacter* GetOwnerAvatarCharacter();

	UPROPERTY()
	ACharacter* OwnerAvatarCharacter;

	void ApplyGameplayEffectToHitResultActor(
		const FHitResult& HitResult, TSubclassOf<UGameplayEffect> GameplayEffect, int Level = 1
	);

	AActor* GetAimTarget(float AimDistance, ETeamAttitude::Type TeamAttitude) const;

	void SendLocalGameplayEvent(const FGameplayTag& EventTag, const FGameplayEventData& EventData);

private:
	FAbilityData* FindAbilityData() const;

	float GetBaseCooldownTime() const;
	float GetBaseCostValue() const;

	bool ValidateCostAttributeConsistency(const FAbilityData* Data) const;

	// 缓存，避免返回局部变量
	mutable FGameplayTagContainer CachedCooldownTags;

	UPROPERTY()
	mutable UDataTable* CachedAbilityDataTable;

public:
	FORCEINLINE bool ShouldDrawDebug() const { return bShouldDrawDebug; }
};

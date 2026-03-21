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

protected:
	UAnimInstance* GetOwnerAnimInstance() const;

	TArray<FHitResult> GetHitResultFromSweepLocationTargetData(
		const FGameplayAbilityTargetDataHandle& TargetDataHandle,
		float SphereSweepRadius = 30.f,
		ETeamAttitude::Type TargetTeam = ETeamAttitude::Hostile, // 目标Team属性
		bool bDrawDebug = false,
		bool bIgnoreSelf = true
	) const;
};

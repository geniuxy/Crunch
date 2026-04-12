// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "CAbilitySystemFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class CRUNCH_API UCAbilitySystemFunctionLibrary : public UAbilitySystemBlueprintLibrary
{
	GENERATED_BODY()

public:
	static float GetStaticCooldownDurationForAbility(const UGameplayAbility* Ability);
	static float GetStaticCostForAbility(const UGameplayAbility* Ability);

	static bool IsHero(const AActor* ActorToCheck);
	static bool IsAbilityAtMaxLevel(const FGameplayAbilitySpec& AbilitySpec);

	static bool CheckAbilityCost(const FGameplayAbilitySpec& AbilitySpec, const UAbilitySystemComponent& ASC);
	static float GetManaCostFor(
		const UGameplayAbility* AbilityCDO, const UAbilitySystemComponent& ASC, int AbilityLevel
	);
	static float GetCooldownDurationFor(
		const UGameplayAbility* AbilityCDO, const UAbilitySystemComponent& ASC, int AbilityLevel
	);
	static float GetCooldownRemainingFor(const UGameplayAbility* AbilityCDO, const UAbilitySystemComponent& ASC);
};

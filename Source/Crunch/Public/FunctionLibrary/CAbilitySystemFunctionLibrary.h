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
};

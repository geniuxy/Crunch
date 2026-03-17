// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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
};

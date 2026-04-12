// Fill out your copyright notice in the Description page of Project Settings.


#include "FunctionLibrary/CAbilitySystemFunctionLibrary.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "CGameplayTags.h"
#include "Abilities/GameplayAbility.h"

float UCAbilitySystemFunctionLibrary::GetStaticCooldownDurationForAbility(const UGameplayAbility* Ability)
{
	if (!Ability) return 0.f;

	UGameplayEffect* CooldownGameplayEffect = Ability->GetCooldownGameplayEffect();
	if (!CooldownGameplayEffect) return 0.f;

	float CooldownDuration = 0.f;
	CooldownGameplayEffect->DurationMagnitude.GetStaticMagnitudeIfPossible(1.f, CooldownDuration);
	return CooldownDuration;
}

float UCAbilitySystemFunctionLibrary::GetStaticCostForAbility(const UGameplayAbility* Ability)
{
	if (!Ability) return 0.f;

	UGameplayEffect* CostGameplayEffect = Ability->GetCostGameplayEffect();
	if (!CostGameplayEffect || CostGameplayEffect->Modifiers.IsEmpty()) return 0.f;

	float Cost = 0.f;
	CostGameplayEffect->Modifiers[0].ModifierMagnitude.GetStaticMagnitudeIfPossible(1.f, Cost);
	return FMath::Abs(Cost);
}

bool UCAbilitySystemFunctionLibrary::IsHero(const AActor* ActorToCheck)
{
	if (const IAbilitySystemInterface* ActorASI = Cast<IAbilitySystemInterface>(ActorToCheck))
	{
		if (UAbilitySystemComponent* ActorASC = ActorASI->GetAbilitySystemComponent())
		{
			return ActorASC->HasMatchingGameplayTag(CGameplayTags::Crunch_Role_Hero);
		}
	}
	return false;
}

bool UCAbilitySystemFunctionLibrary::IsAbilityAtMaxLevel(const FGameplayAbilitySpec& AbilitySpec)
{
	return AbilitySpec.Level >= 4;
}

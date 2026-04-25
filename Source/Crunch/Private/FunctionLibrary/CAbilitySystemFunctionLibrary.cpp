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

bool UCAbilitySystemFunctionLibrary::CheckAbilityCost(
	const FGameplayAbilitySpec& AbilitySpec, const UAbilitySystemComponent& ASC)
{
	if (const UGameplayAbility* AbilityCDO = AbilitySpec.Ability)
	{
		return AbilityCDO->CheckCost(AbilitySpec.Handle, ASC.AbilityActorInfo.Get());
	}

	return false;
}

bool UCAbilitySystemFunctionLibrary::CheckAbilityCostStatic(
	const UGameplayAbility* AbilityCDO, const UAbilitySystemComponent& ASC)
{
	if (AbilityCDO)
	{
		return AbilityCDO->CheckCost(FGameplayAbilitySpecHandle(), ASC.AbilityActorInfo.Get());
	}

	return false;
}

float UCAbilitySystemFunctionLibrary::GetManaCostFor(
	const UGameplayAbility* AbilityCDO, const UAbilitySystemComponent& ASC, int AbilityLevel)
{
	float ManaCost = 0.f;
	if (AbilityCDO)
	{
		if (UGameplayEffect* CostGameplayEffect = AbilityCDO->GetCostGameplayEffect())
		{
			FGameplayEffectSpecHandle EffectSpecHandle =
				ASC.MakeOutgoingSpec(CostGameplayEffect->GetClass(), AbilityLevel, ASC.MakeEffectContext());
			CostGameplayEffect->Modifiers[0].ModifierMagnitude.AttemptCalculateMagnitude(
				*EffectSpecHandle.Data.Get(), ManaCost
			);
		}
	}

	return FMath::Abs(ManaCost);
}

float UCAbilitySystemFunctionLibrary::GetCooldownDurationFor(
	const UGameplayAbility* AbilityCDO, const UAbilitySystemComponent& ASC, int AbilityLevel)
{
	float CooldownDuration = 0.f;
	if (AbilityCDO)
	{
		if (UGameplayEffect* CooldownGameplayEffect = AbilityCDO->GetCooldownGameplayEffect())
		{
			FGameplayEffectSpecHandle EffectSpecHandle =
				ASC.MakeOutgoingSpec(CooldownGameplayEffect->GetClass(), AbilityLevel, ASC.MakeEffectContext());
			CooldownGameplayEffect->DurationMagnitude.AttemptCalculateMagnitude(
				*EffectSpecHandle.Data.Get(), CooldownDuration
			);
		}
	}

	return FMath::Abs(CooldownDuration);
}

float UCAbilitySystemFunctionLibrary::GetCooldownRemainingFor(
	const UGameplayAbility* AbilityCDO, const UAbilitySystemComponent& ASC)
{
	if (!AbilityCDO) return 0.f;

	UGameplayEffect* CooldownEffect = AbilityCDO->GetCooldownGameplayEffect();
	if (!CooldownEffect) return 0.f;

	FGameplayEffectQuery CooldownEffectQuery;
	CooldownEffectQuery.EffectDefinition = CooldownEffect->GetClass();

	float CooldownRemaining = 0.f;
	FJsonSerializableArrayFloat CooldownTimeRemainings = ASC.GetActiveEffectsTimeRemaining(CooldownEffectQuery);

	for (const float Remaining : CooldownTimeRemainings)
	{
		if (Remaining > CooldownRemaining)
		{
			CooldownRemaining = Remaining;
		}
	}

	return CooldownRemaining;
}

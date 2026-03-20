// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/CAbilitySystemComponent.h"

#include "GAS/CAttributeSet.h"

UCAbilitySystemComponent::UCAbilitySystemComponent()
{
	GetGameplayAttributeValueChangeDelegate(UCAttributeSet::GetHealthAttribute()).AddUObject(
		this, &ThisClass::HealthUpdated);
}

void UCAbilitySystemComponent::InitializeBaseAttributes()
{
	if (!GetOwner() || !GetOwner()->HasAuthority()) return;

	for (const TSubclassOf<UGameplayEffect>& EffectClass : InitializeGameplayEffects)
	{
		AuthApplyGameplayEffect(EffectClass);
	}
}

void UCAbilitySystemComponent::GiveInitialAbilities()
{
	if (!GetOwner() || !GetOwner()->HasAuthority()) return;

	for (const TPair<ECAbilityInputID, TSubclassOf<UGameplayAbility>>& AbilityPair : Abilities)
	{
		GiveAbility(FGameplayAbilitySpec(AbilityPair.Value, 0, (int32)AbilityPair.Key, nullptr));
	}

	for (const TPair<ECAbilityInputID, TSubclassOf<UGameplayAbility>>& AbilityPair : BasicAbilities)
	{
		GiveAbility(FGameplayAbilitySpec(AbilityPair.Value, 1, (int32)AbilityPair.Key, nullptr));
	}
}

void UCAbilitySystemComponent::ApplyFullStatsEffect()
{
	if (FullStatsEffect)
	{
		AuthApplyGameplayEffect(FullStatsEffect);
	}
}

void UCAbilitySystemComponent::AuthApplyGameplayEffect(TSubclassOf<UGameplayEffect> GameplayEffect, int Level)
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		FGameplayEffectSpecHandle SpecHandle = MakeOutgoingSpec(GameplayEffect, Level, MakeEffectContext());
		ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}
}

void UCAbilitySystemComponent::HealthUpdated(const FOnAttributeChangeData& ChangeData)
{
	if (!GetOwner() || !GetOwner()->HasAuthority()) return;

	if (ChangeData.NewValue <= 0.f && DeathEffect)
	{
		AuthApplyGameplayEffect(DeathEffect);
	}
}

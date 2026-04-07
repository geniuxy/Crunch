// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/CAbilitySystemComponent.h"

#include "CTypes/CStruct.h"
#include "GAS/CAttributeSet.h"
#include "GAS/CHeroAttributeSet.h"

UCAbilitySystemComponent::UCAbilitySystemComponent()
{
	GetGameplayAttributeValueChangeDelegate(UCAttributeSet::GetHealthAttribute()).AddUObject(
		this, &ThisClass::HealthUpdated
	);

	GenericConfirmInputID = (int32)ECAbilityInputID::Confirm;
	GenericCancelInputID = (int32)ECAbilityInputID::Cancel;
}

void UCAbilitySystemComponent::ServerSideInit()
{
	InitializeBaseAttributes();
	InitializeBaseGameplayEffects();
	GiveInitialAbilities();
}

void UCAbilitySystemComponent::ApplyFullStatsEffect()
{
	if (FullStatsEffect)
	{
		AuthApplyGameplayEffect(FullStatsEffect);
	}
}

void UCAbilitySystemComponent::DisableAim()
{
	if (!GetOwner() || !GetOwner()->HasAuthority()) return;
	
	AuthApplyGameplayEffect(DisableAimEffect);
}

void UCAbilitySystemComponent::InitializeBaseAttributes()
{
	if (!BaseStatDataTable || !GetOwner() || !GetOwner()->HasAuthority()) return;

	const FHeroBaseStats* BaseStats = nullptr;
	for (const TPair<FName, uint8*>& DataPair : BaseStatDataTable->GetRowMap())
	{
		BaseStats = BaseStatDataTable->FindRow<FHeroBaseStats>(DataPair.Key, "");
		if (BaseStats && BaseStats->HeroClass == GetOwner()->GetClass())
		{
			break;
		}
	}

	if (BaseStats)
	{
		SetNumericAttributeBase(UCAttributeSet::GetMaxHealthAttribute(), BaseStats->BaseMaxHealth);
		SetNumericAttributeBase(UCAttributeSet::GetMaxManaAttribute(), BaseStats->BaseMaxMana);
		SetNumericAttributeBase(UCAttributeSet::GetAttackDamageAttribute(), BaseStats->BaseAttackDamage);
		SetNumericAttributeBase(UCAttributeSet::GetArmorAttribute(), BaseStats->BaseArmor);
		SetNumericAttributeBase(UCAttributeSet::GetMoveSpeedAttribute(), BaseStats->BaseMoveSpeed);
		
		SetNumericAttributeBase(UCHeroAttributeSet::GetStrengthAttribute(), BaseStats->Strength);
		SetNumericAttributeBase(UCHeroAttributeSet::GetStrengthGrowthRateAttribute(), BaseStats->StrengthGrowthRate);
		SetNumericAttributeBase(UCHeroAttributeSet::GetIntelligenceAttribute(), BaseStats->Intelligence);
		SetNumericAttributeBase(UCHeroAttributeSet::GetIntelligenceGrowthRateAttribute(), BaseStats->IntelligenceGrowthRate);
	}
}

void UCAbilitySystemComponent::InitializeBaseGameplayEffects()
{
	if (!GetOwner() || !GetOwner()->HasAuthority()) return;

	for (const TSubclassOf<UGameplayEffect>& EffectClass : InitialGameplayEffects)
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

// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/CAbilitySystemComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "CGameplayTags.h"
#include "GameplayEffectExtension.h"
#include "CTypes/CStruct.h"
#include "GAS/CAttributeSet.h"
#include "GAS/CHeroAttributeSet.h"
#include "GAS/Data/PA_AbilitySystemGenerics.h"

UCAbilitySystemComponent::UCAbilitySystemComponent()
{
	GetGameplayAttributeValueChangeDelegate(UCAttributeSet::GetHealthAttribute()).AddUObject(
		this, &ThisClass::HealthUpdated
	);
	GetGameplayAttributeValueChangeDelegate(UCAttributeSet::GetManaAttribute()).AddUObject(
		this, &ThisClass::ManaUpdated
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
	if (AbilitySystemGenerics && AbilitySystemGenerics->GetFullStatsEffect())
	{
		AuthApplyGameplayEffect(AbilitySystemGenerics->GetFullStatsEffect());
	}
}

void UCAbilitySystemComponent::DisableAim()
{
	if (!GetOwner() || !GetOwner()->HasAuthority()) return;

	if (AbilitySystemGenerics && AbilitySystemGenerics->GetDisableAimEffect())
	{
		AuthApplyGameplayEffect(AbilitySystemGenerics->GetDisableAimEffect());
	}
}

void UCAbilitySystemComponent::InitializeBaseAttributes()
{
	if (!AbilitySystemGenerics || !AbilitySystemGenerics->GetBaseStatDataTable() ||
		!GetOwner() || !GetOwner()->HasAuthority())
	{
		return;
	}

	UDataTable* BaseStatDataTable = AbilitySystemGenerics->GetBaseStatDataTable();
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
		SetNumericAttributeBase(UCHeroAttributeSet::GetIntelligenceGrowthRateAttribute(),
		                        BaseStats->IntelligenceGrowthRate);
	}
}

void UCAbilitySystemComponent::InitializeBaseGameplayEffects()
{
	if (!AbilitySystemGenerics || !GetOwner() || !GetOwner()->HasAuthority()) return;

	for (const TSubclassOf<UGameplayEffect>& EffectClass : AbilitySystemGenerics->GetInitialGameplayEffects())
	{
		AuthApplyGameplayEffect(EffectClass);
	}
}

void UCAbilitySystemComponent::GiveInitialAbilities()
{
	if (!AbilitySystemGenerics || !GetOwner() || !GetOwner()->HasAuthority()) return;

	for (const TPair<ECAbilityInputID, TSubclassOf<UGameplayAbility>>& AbilityPair : Abilities)
	{
		GiveAbility(FGameplayAbilitySpec(AbilityPair.Value, 0, (int32)AbilityPair.Key, nullptr));
	}

	for (const TPair<ECAbilityInputID, TSubclassOf<UGameplayAbility>>& AbilityPair : BasicAbilities)
	{
		GiveAbility(FGameplayAbilitySpec(AbilityPair.Value, 1, (int32)AbilityPair.Key, nullptr));
	}

	for (const TSubclassOf<UGameplayAbility>& PassiveAbility : AbilitySystemGenerics->GetPassiveAbilities())
	{
		GiveAbility(FGameplayAbilitySpec(PassiveAbility, 1, -1, nullptr));
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
	if (!AbilitySystemGenerics || !GetOwner() || !GetOwner()->HasAuthority()) return;

	bool bFound = false;
	float MaxHealth = GetGameplayAttributeValue(UCAttributeSet::GetMaxHealthAttribute(), bFound);
	if (bFound && ChangeData.NewValue >= MaxHealth)
	{
		if (!HasMatchingGameplayTag(CGameplayTags::Crunch_Stats_Health_Full))
		{
			// 只在本地执行Tag添加
			AddLooseGameplayTag(CGameplayTags::Crunch_Stats_Health_Full);
		}
	}
	else
	{
		RemoveLooseGameplayTag(CGameplayTags::Crunch_Stats_Health_Full);
	}

	if (ChangeData.NewValue <= 0.f)
	{
		if (!HasMatchingGameplayTag(CGameplayTags::Crunch_Stats_Health_Empty))
		{
			AddLooseGameplayTag(CGameplayTags::Crunch_Stats_Health_Empty);

			if (AbilitySystemGenerics->GetDeathEffect())
			{
				AuthApplyGameplayEffect(AbilitySystemGenerics->GetDeathEffect());
			}

			FGameplayEventData DeadAbilityEventData;
			if (ChangeData.GEModData)
			{
				DeadAbilityEventData.ContextHandle = ChangeData.GEModData->EffectSpec.GetContext();
			}

			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
				GetOwner(), CGameplayTags::Crunch_Stats_Dead, DeadAbilityEventData
			);
		}
	}
	else
	{
		RemoveLooseGameplayTag(CGameplayTags::Crunch_Stats_Health_Empty);
	}
}

void UCAbilitySystemComponent::ManaUpdated(const FOnAttributeChangeData& ChangeData)
{
	if (!AbilitySystemGenerics || !GetOwner() || !GetOwner()->HasAuthority()) return;

	bool bFound = false;
	float MaxMana = GetGameplayAttributeValue(UCAttributeSet::GetMaxManaAttribute(), bFound);
	if (bFound && ChangeData.NewValue >= MaxMana)
	{
		if (!HasMatchingGameplayTag(CGameplayTags::Crunch_Stats_Mana_Full))
		{
			// 只在本地执行Tag添加
			AddLooseGameplayTag(CGameplayTags::Crunch_Stats_Mana_Full);
		}
	}
	else
	{
		RemoveLooseGameplayTag(CGameplayTags::Crunch_Stats_Mana_Full);
	}

	if (ChangeData.NewValue <= 0.f)
	{
		if (!HasMatchingGameplayTag(CGameplayTags::Crunch_Stats_Mana_Empty))
		{
			AddLooseGameplayTag(CGameplayTags::Crunch_Stats_Mana_Empty);

			if (AbilitySystemGenerics->GetDeathEffect())
			{
				AuthApplyGameplayEffect(AbilitySystemGenerics->GetDeathEffect());
			}
		}
	}
	else
	{
		RemoveLooseGameplayTag(CGameplayTags::Crunch_Stats_Mana_Empty);
	}
}

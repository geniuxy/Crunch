// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/CAbilitySystemComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "CGameplayTags.h"
#include "CrunchDebugHelper.h"
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
	GetGameplayAttributeValueChangeDelegate(UCHeroAttributeSet::GetExperienceAttribute()).AddUObject(
		this, &ThisClass::ExperienceUpdated
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

bool UCAbilitySystemComponent::IsAtMaxLevel() const
{
	bool bFound;
	float CurrentLevel = GetGameplayAttributeValue(UCHeroAttributeSet::GetLevelAttribute(), bFound);
	float MaxLevel = GetGameplayAttributeValue(UCHeroAttributeSet::GetMaxLevelAttribute(), bFound);
	return CurrentLevel >= MaxLevel;
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
		SetNumericAttributeBase(
			UCHeroAttributeSet::GetIntelligenceGrowthRateAttribute(), BaseStats->IntelligenceGrowthRate
		);
	}

	const FRealCurve* ExperienceCurve = AbilitySystemGenerics->GetExperienceCurve();
	if (ExperienceCurve)
	{
		int32 MaxLevel = ExperienceCurve->GetNumKeys();
		SetNumericAttributeBase(UCHeroAttributeSet::GetMaxLevelAttribute(), MaxLevel);

		float MaxLevelExp = ExperienceCurve->GetKeyValue(ExperienceCurve->GetLastKeyHandle());
		SetNumericAttributeBase(UCHeroAttributeSet::GetMaxLevelExperienceAttribute(), MaxLevelExp);

		Debug::Print(FString::Printf(TEXT("Max Level is: %d, max experience is: %f"), MaxLevel, MaxLevelExp));
	}

	ExperienceUpdated(FOnAttributeChangeData()); // 经验值为0
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

void UCAbilitySystemComponent::ExperienceUpdated(const FOnAttributeChangeData& ChangeData)
{
	if (!GetOwner() || !GetOwner()->HasAuthority()) return;
	if (IsAtMaxLevel()) return;
	if (!AbilitySystemGenerics) return;

	float CurrentExp = ChangeData.NewValue;
	const FRealCurve* ExperienceCurve = AbilitySystemGenerics->GetExperienceCurve();
	checkf(ExperienceCurve, TEXT("找不到ExperienceCurve"));

	float PrevLevelExp = 0.f;
	float NextLevelExp = 0.f;
	int32 NewLevel = 1;
	for (auto Iter = ExperienceCurve->GetKeyHandleIterator(); Iter; ++Iter)
	{
		float ExperienceToReachLevel = ExperienceCurve->GetKeyValue(*Iter);
		if (CurrentExp < ExperienceToReachLevel)
		{
			NextLevelExp = ExperienceToReachLevel;
			break;
		}

		PrevLevelExp = ExperienceToReachLevel;
		NewLevel = Iter.GetIndex() + 1;
	}

	float CurrentLevel = GetNumericAttributeBase(UCHeroAttributeSet::GetLevelAttribute());
	float CurrentUpgradePoint = GetNumericAttributeBase(UCHeroAttributeSet::GetUpgradePointAttribute());

	float LevelUpgraded = NewLevel - CurrentLevel;
	float NewUpgradePoint = CurrentUpgradePoint + LevelUpgraded;

	SetNumericAttributeBase(UCHeroAttributeSet::GetLevelAttribute(), NewLevel);
	SetNumericAttributeBase(UCHeroAttributeSet::GetPrevLevelExperienceAttribute(), PrevLevelExp);
	SetNumericAttributeBase(UCHeroAttributeSet::GetNextLevelExperienceAttribute(), NextLevelExp);
	SetNumericAttributeBase(UCHeroAttributeSet::GetUpgradePointAttribute(), NewUpgradePoint);
}

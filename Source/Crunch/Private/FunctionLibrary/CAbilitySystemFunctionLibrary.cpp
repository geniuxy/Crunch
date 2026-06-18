// Fill out your copyright notice in the Description page of Project Settings.


#include "FunctionLibrary/CAbilitySystemFunctionLibrary.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystemInterface.h"
#include "CGameplayTags.h"
#include "GameplayCueManager.h"
#include "Abilities/GameplayAbility.h"
#include "CTypes/CStruct.h"
#include "FrameWork/EngineSubsystem/CDataSubsystem.h"

float UCAbilitySystemFunctionLibrary::GetStaticCooldownDurationForAbility(const UGameplayAbility* Ability)
{
	if (!Ability) return 0.f;

	float CooldownDuration = 0.f;
	if (UCDataSubsystem* DataSubsystem = UCDataSubsystem::Get())
	{
		UDataTable* AbilityDataTable = DataSubsystem->GetAbilityDataTable();
		for (const auto& AbilityDataRowName : AbilityDataTable->GetRowNames())
		{
			FAbilityData* WidgetData = AbilityDataTable->FindRow<FAbilityData>(AbilityDataRowName, "");
			if (WidgetData->AbilityClass == Ability->GetClass())
			{
				CooldownDuration = WidgetData->CooldownTime.AsInteger(1);
				break;
			}
		}
	}
	else if (UGameplayEffect* CooldownGameplayEffect = Ability->GetCooldownGameplayEffect())
	{
		CooldownGameplayEffect->DurationMagnitude.GetStaticMagnitudeIfPossible(1.f, CooldownDuration);
	}

	return CooldownDuration;
}

float UCAbilitySystemFunctionLibrary::GetStaticCostForAbility(const UGameplayAbility* Ability)
{
	if (!Ability) return 0.f;

	float Cost = 0.f;
	UGameplayEffect* CostGameplayEffect = Ability->GetCostGameplayEffect();
	if (UCDataSubsystem* DataSubsystem = UCDataSubsystem::Get())
	{
		UDataTable* AbilityDataTable = DataSubsystem->GetAbilityDataTable();
		for (const auto& AbilityDataRowName : AbilityDataTable->GetRowNames())
		{
			FAbilityData* WidgetData = AbilityDataTable->FindRow<FAbilityData>(AbilityDataRowName, "");
			if (WidgetData->AbilityClass == Ability->GetClass())
			{
				Cost = WidgetData->CostValue.AsInteger(1);
				break;
			}
		}
	}
	else if (CostGameplayEffect && !CostGameplayEffect->Modifiers.IsEmpty())
	{
		CostGameplayEffect->Modifiers[0].ModifierMagnitude.GetStaticMagnitudeIfPossible(1.f, Cost);
	}

	return FMath::Abs(Cost);
}

bool UCAbilitySystemFunctionLibrary::IsActorDead(const AActor* ActorToCheck)
{
	return ActorHasTag(ActorToCheck, CGameplayTags::Crunch_Stats_Dead);
}

bool UCAbilitySystemFunctionLibrary::IsHero(const AActor* ActorToCheck)
{
	return ActorHasTag(ActorToCheck, CGameplayTags::Crunch_Role_Hero);
}

bool UCAbilitySystemFunctionLibrary::ActorHasTag(const AActor* ActorToCheck, const FGameplayTag& Tag)
{
	if (const IAbilitySystemInterface* ActorASI = Cast<IAbilitySystemInterface>(ActorToCheck))
	{
		if (UAbilitySystemComponent* ActorASC = ActorASI->GetAbilitySystemComponent())
		{
			return ActorASC->HasMatchingGameplayTag(Tag);
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

float UCAbilitySystemFunctionLibrary::GetCostValueFor(
	const UGameplayAbility* AbilityCDO, const UAbilitySystemComponent& ASC, int AbilityLevel)
{
	float CostValue = 0.f;
	if (AbilityCDO)
	{
		if (UCDataSubsystem* DataSubsystem = UCDataSubsystem::Get())
		{
			UDataTable* AbilityDataTable = DataSubsystem->GetAbilityDataTable();
			for (const auto& AbilityDataRowName : AbilityDataTable->GetRowNames())
			{
				FAbilityData* WidgetData = AbilityDataTable->FindRow<FAbilityData>(AbilityDataRowName, "");
				if (WidgetData->AbilityClass == AbilityCDO->GetClass())
				{
					float CostReduction = ASC.GetNumericAttribute(UCAttributeSet::GetCostReductionAttribute());
					CostValue = WidgetData->CostValue.AsInteger(AbilityLevel) *
						(1.0f - FMath::Clamp(CostReduction, 0.f, 1.0f));
					break;
				}
			}
		}
		else if (UGameplayEffect* CostGameplayEffect = AbilityCDO->GetCostGameplayEffect())
		{
			FGameplayEffectSpecHandle EffectSpecHandle =
				ASC.MakeOutgoingSpec(CostGameplayEffect->GetClass(), AbilityLevel, ASC.MakeEffectContext());
			CostGameplayEffect->Modifiers[0].ModifierMagnitude.AttemptCalculateMagnitude(
				*EffectSpecHandle.Data.Get(), CostValue
			);
		}
	}

	return FMath::Abs(CostValue);
}

float UCAbilitySystemFunctionLibrary::GetCooldownDurationFor(
	const UGameplayAbility* AbilityCDO, const UAbilitySystemComponent& ASC, int AbilityLevel)
{
	float CooldownDuration = 0.f;
	if (AbilityCDO)
	{
		if (UCDataSubsystem* DataSubsystem = UCDataSubsystem::Get())
		{
			UDataTable* AbilityDataTable = DataSubsystem->GetAbilityDataTable();
			for (const auto& AbilityDataRowName : AbilityDataTable->GetRowNames())
			{
				FAbilityData* WidgetData = AbilityDataTable->FindRow<FAbilityData>(AbilityDataRowName, "");
				if (WidgetData->AbilityClass == AbilityCDO->GetClass())
				{
					float CooldownReduction = ASC.GetNumericAttribute(UCAttributeSet::GetCooldownReductionAttribute());
					CooldownDuration = WidgetData->CooldownTime.AsInteger(AbilityLevel) *
						(1.0f - FMath::Clamp(CooldownReduction, -1.0f, 1.0f));
					break;
				}
			}
		}
		else if (UGameplayEffect* CooldownGameplayEffect = AbilityCDO->GetCooldownGameplayEffect())
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

float UCAbilitySystemFunctionLibrary::GetCooldownRemainingFor(
	const UGameplayEffect* CooldownEffect, const UAbilitySystemComponent& ASC)
{
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

FGameplayTag UCAbilitySystemFunctionLibrary::GetCooldownTagFor(const FActiveGameplayEffect* CooldownEffect)
{
	FGameplayTag OutTag;
	if (CooldownEffect)
	{
		FGameplayTagContainer CooldownTags = CooldownEffect->Spec.GetDynamicAssetTags();
		for (const FGameplayTag& CooldownTag : CooldownTags.GetGameplayTagArray())
		{
			if (CooldownTag.MatchesTag(CGameplayTags::Crunch_Ability_Cooldown))
			{
				OutTag = CooldownTag;
				break;
			}
		}
	}
	return OutTag;
}

FGameplayTag UCAbilitySystemFunctionLibrary::GetCooldownTagFor(const UGameplayAbility* CooldownGA)
{
	FGameplayTag OutTag;
	if (UCDataSubsystem* DataSubsystem = UCDataSubsystem::Get())
	{
		UDataTable* AbilityDataTable = DataSubsystem->GetAbilityDataTable();
		for (const auto& AbilityDataRowName : AbilityDataTable->GetRowNames())
		{
			FAbilityData* WidgetData = AbilityDataTable->FindRow<FAbilityData>(AbilityDataRowName, "");
			if (WidgetData->AbilityClass == CooldownGA->GetClass())
			{
				OutTag = WidgetData->CooldownTag;
				break;
			}
		}
	}
	return OutTag;
}

void UCAbilitySystemFunctionLibrary::SendLocalGameplayCue(
	AActor* CueTargetActor, const FHitResult& HitResult, const FGameplayTag& GameplayCueTag)
{
	FGameplayCueParameters CueParams;
	CueParams.Location = HitResult.ImpactPoint;
	CueParams.Normal = HitResult.ImpactNormal;

	UAbilitySystemGlobals::Get().GetGameplayCueManager()->HandleGameplayCue(
		CueTargetActor, GameplayCueTag, EGameplayCueEvent::Executed, CueParams
	);
}

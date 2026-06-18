// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Abilities/AbilityGauge.h"

#include "AbilitySystemComponent.h"
#include "Abilities/GameplayAbility.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "CTypes/CStruct.h"
#include "FunctionLibrary/CAbilitySystemFunctionLibrary.h"
#include "GAS/CAbilitySystemComponent.h"
#include "GAS/CAttributeSet.h"
#include "GAS/CHeroAttributeSet.h"
#include "Widgets/Abilities/AbilityToolTip.h"

void UAbilityGauge::NativeConstruct()
{
	Super::NativeConstruct();

	CooldownCounterText->SetVisibility(ESlateVisibility::Hidden);

	OwnerAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwningPlayerPawn());
	if (OwnerAbilitySystemComponent)
	{
		OwnerAbilitySystemComponent->AbilityCommittedCallbacks.AddUObject(this, &ThisClass::AbilityCommit);
		OwnerAbilitySystemComponent->AbilitySpecDirtiedCallbacks.AddUObject(this, &ThisClass::AbilitySpecUpdated);
		OwnerAbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
			UCHeroAttributeSet::GetUpgradePointAttribute()).AddUObject(this, &ThisClass::UpgradePointUpdated
		);
		OwnerAbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
			UCAttributeSet::GetManaAttribute()).AddUObject(this, &ThisClass::ManaUpdated
		);
		OwnerAbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
			UCAttributeSet::GetCooldownReductionAttribute()).AddUObject(this, &ThisClass::CooldownReductionUpdated
		);
		OwnerAbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
			UCAttributeSet::GetCostReductionAttribute()).AddUObject(this, &ThisClass::CostReduction
		);
		if (UCAbilitySystemComponent* OwnerASC = Cast<UCAbilitySystemComponent>(OwnerAbilitySystemComponent))
		{
			OwnerASC->OnCooldownTimeUpdated.AddUObject(this, &ThisClass::CooldownTimeUpdated);
		}
	}

	WholeNumberFormattingOptions.MaximumFractionalDigits = 0;
	OneDigitNumberFormattingOptions.MaximumFractionalDigits = 1;
}

void UAbilityGauge::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);
	AbilityCDO = Cast<UGameplayAbility>(ListItemObject);

	float CooldownDuration = UCAbilitySystemFunctionLibrary::GetStaticCooldownDurationForAbility(AbilityCDO);
	float Cost = UCAbilitySystemFunctionLibrary::GetStaticCostForAbility(AbilityCDO);

	CooldownDurationText->SetText(FText::AsNumber(CooldownDuration));
	CostText->SetText(FText::AsNumber(Cost));
	AbilityLevelGauge->GetDynamicMaterial()->SetScalarParameterValue(AbilityLevelParamName, 0);

	if (OwnerAbilitySystemComponent)
	{
		bool bFound = false;
		float UpgradePoint = OwnerAbilitySystemComponent->GetGameplayAttributeValue(
			UCHeroAttributeSet::GetUpgradePointAttribute(), bFound
		);
		if (bFound)
		{
			FOnAttributeChangeData Data;
			Data.NewValue = UpgradePoint;
			UpgradePointUpdated(Data);
		}
	}
}

void UAbilityGauge::ConfigureWithWidgetData(const FAbilityData* WidgetData)
{
	if (Icon && WidgetData)
	{
		Icon->GetDynamicMaterial()->SetTextureParameterValue(IconMaterialParamName, WidgetData->Icon.LoadSynchronous());
		CreateToolTipWidget(WidgetData);
	}
}

void UAbilityGauge::AbilityCommit(UGameplayAbility* Ability)
{
	if (Ability->GetClass()->GetDefaultObject() == AbilityCDO)
	{
		float CooldownTimeRemaining = 0.f;
		float CooldownDuration = 0.f;

		Ability->GetCooldownTimeRemainingAndDuration(
			Ability->GetCurrentAbilitySpecHandle(),
			Ability->GetCurrentActorInfo(),
			CooldownTimeRemaining,
			CooldownDuration
		);

		StartCooldown(CooldownTimeRemaining, CooldownDuration);
	}
}

void UAbilityGauge::StartCooldown(float CooldownTimeRemaining, float CooldownDuration)
{
	CooldownDurationText->SetText(FText::AsNumber(CooldownDuration));
	CachedCooldownDuration = CooldownDuration;
	CachedCooldownTimeRemaining = CooldownTimeRemaining;

	CooldownCounterText->SetVisibility(ESlateVisibility::Visible);

	GetWorld()->GetTimerManager().SetTimer(
		CooldownTimerUpdateHandle, this, &ThisClass::UpdateCooldown, CooldownUpdateInterval, true, 0.f
	);
}

void UAbilityGauge::CooldownFinished()
{
	CachedCooldownDuration = 0.f;
	CachedCooldownTimeRemaining = 0.f;
	CachedCooldownReduction = 0.f;
	CooldownCounterText->SetVisibility(ESlateVisibility::Hidden);
	GetWorld()->GetTimerManager().ClearTimer(CooldownTimerUpdateHandle);
	Icon->GetDynamicMaterial()->SetScalarParameterValue(CooldownPercentParamName, 1.f);
}

void UAbilityGauge::UpdateCooldown()
{
	CachedCooldownTimeRemaining -= CooldownUpdateInterval;
	FNumberFormattingOptions* FormattingOptions =
		CachedCooldownTimeRemaining > 1.f ? &WholeNumberFormattingOptions : &OneDigitNumberFormattingOptions;
	if (CachedCooldownTimeRemaining >= 0.05f)
	{
		CooldownCounterText->SetText(FText::AsNumber(CachedCooldownTimeRemaining, FormattingOptions));
	}
	else
	{
		CooldownFinished();
	}
	Icon->GetDynamicMaterial()->SetScalarParameterValue(
		CooldownPercentParamName, 1.f - CachedCooldownTimeRemaining / CachedCooldownDuration
	);
}

const FGameplayAbilitySpec* UAbilityGauge::GetAbilitySpec()
{
	if (!AbilityCDO || !OwnerAbilitySystemComponent)
	{
		return nullptr;
	}
	if (!CachedAbilitySpecHandle.IsValid())
	{
		FGameplayAbilitySpec* FoundAbilitySpec = OwnerAbilitySystemComponent->FindAbilitySpecFromClass(
			AbilityCDO->GetClass());
		CachedAbilitySpecHandle = FoundAbilitySpec->Handle;
		return FoundAbilitySpec;
	}

	return OwnerAbilitySystemComponent->FindAbilitySpecFromClass(AbilityCDO->GetClass());
}

void UAbilityGauge::AbilitySpecUpdated(const FGameplayAbilitySpec& AbilitySpec)
{
	if (AbilitySpec.Ability != AbilityCDO) return;

	bIsAbilityLearned = AbilitySpec.Level > 0;
	AbilityLevelGauge->GetDynamicMaterial()->SetScalarParameterValue(AbilityLevelParamName, AbilitySpec.Level);
	UpdateCanCast();

	float NewCooldownDuration = UCAbilitySystemFunctionLibrary::GetCooldownDurationFor(
		AbilityCDO, *OwnerAbilitySystemComponent, AbilitySpec.Level
	);
	float NewCost = UCAbilitySystemFunctionLibrary::GetCostValueFor(
		AbilityCDO, *OwnerAbilitySystemComponent, AbilitySpec.Level
	);
	CooldownDurationText->SetText(FText::AsNumber(NewCooldownDuration));
	CostText->SetText(FText::AsNumber(NewCost));
}

void UAbilityGauge::UpdateCanCast()
{
	const FGameplayAbilitySpec* AbilitySpec = GetAbilitySpec();
	bool bCanCast = bIsAbilityLearned;
	if (AbilitySpec)
	{
		if (OwnerAbilitySystemComponent &&
			!UCAbilitySystemFunctionLibrary::CheckAbilityCost(*AbilitySpec, *OwnerAbilitySystemComponent))
		{
			bCanCast = false;
		}
	}

	Icon->GetDynamicMaterial()->SetScalarParameterValue(CanCastAbilityParamName, bCanCast);
}

void UAbilityGauge::UpgradePointUpdated(const FOnAttributeChangeData& Data)
{
	bool bHasUpgradePoint = Data.NewValue > 0;
	bool bCanUpgrade = false;
	bool bAbilityAtMaxLevel = false;
	if (const FGameplayAbilitySpec* AbilitySpec = GetAbilitySpec())
	{
		if (UCAbilitySystemFunctionLibrary::IsAbilityAtMaxLevel(*AbilitySpec))
		{
			bAbilityAtMaxLevel = true;
		}

		bool bFound;
		float OwnerLevel = OwnerAbilitySystemComponent->GetGameplayAttributeValue(
			UCHeroAttributeSet::GetLevelAttribute(), bFound
		);
		bCanUpgrade = OwnerLevel >= 2 * AbilitySpec->Level + 1;
	}

	Icon->GetDynamicMaterial()->SetScalarParameterValue(
		UpgradePointAvaliableParamName, bHasUpgradePoint && bCanUpgrade && !bAbilityAtMaxLevel
	);
}

void UAbilityGauge::ManaUpdated(const FOnAttributeChangeData& Data)
{
	UpdateCanCast();
}

void UAbilityGauge::CooldownReductionUpdated(const FOnAttributeChangeData& Data)
{
	if (const FGameplayAbilitySpec* AbilitySpec = GetAbilitySpec())
	{
		float NewCooldownDuration = UCAbilitySystemFunctionLibrary::GetCooldownDurationFor(
			AbilityCDO, *OwnerAbilitySystemComponent, AbilitySpec->Level
		);
		CooldownDurationText->SetText(FText::AsNumber(NewCooldownDuration));
	}
}

void UAbilityGauge::CostReduction(const FOnAttributeChangeData& Data)
{
	if (const FGameplayAbilitySpec* AbilitySpec = GetAbilitySpec())
	{
		float NewCostDuration = UCAbilitySystemFunctionLibrary::GetCostValueFor(
			AbilityCDO, *OwnerAbilitySystemComponent, AbilitySpec->Level
		);
		CostText->SetText(FText::AsNumber(NewCostDuration));

		UpdateCanCast();
	}
}

void UAbilityGauge::CooldownTimeUpdated(
	UGameplayAbility* Ability, float NewRemainingTime, float NewDuration)
{
	const FGameplayAbilitySpec* AbilitySpec = GetAbilitySpec();
	if (!AbilitySpec) return;
	if (AbilitySpec->Ability != Ability) return;

	if (CachedCooldownTimeRemaining == 0.f) return;

	CachedCooldownTimeRemaining = NewRemainingTime;
	CachedCooldownDuration = NewDuration;
}

void UAbilityGauge::CreateToolTipWidget(const FAbilityData* AbilityData)
{
	if (!AbilityData || !AbilityToolTipClass || !AbilityCDO) return;

	UAbilityToolTip* InstantiatedToolTip = CreateWidget<UAbilityToolTip>(GetOwningPlayer(), AbilityToolTipClass);
	if (InstantiatedToolTip)
	{
		float CooldownDuration = UCAbilitySystemFunctionLibrary::GetStaticCooldownDurationForAbility(AbilityCDO);
		float Cost = UCAbilitySystemFunctionLibrary::GetStaticCostForAbility(AbilityCDO);
		InstantiatedToolTip->SetAbilityInfo(
			AbilityData->AbilityName,
			AbilityData->Icon.LoadSynchronous(),
			AbilityData->Description,
			CooldownDuration,
			Cost
		);

		SetToolTip(InstantiatedToolTip);
	}
}

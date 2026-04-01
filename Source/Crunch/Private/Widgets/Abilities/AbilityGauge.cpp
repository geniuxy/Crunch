// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Abilities/AbilityGauge.h"

#include "AbilitySystemComponent.h"
#include "Abilities/GameplayAbility.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "CTypes/CStruct.h"
#include "FunctionLibrary/CAbilitySystemFunctionLibrary.h"

void UAbilityGauge::NativeConstruct()
{
	Super::NativeConstruct();

	CooldownCounterText->SetVisibility(ESlateVisibility::Hidden);

	UAbilitySystemComponent* OwnerASC =
		UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwningPlayerPawn());
	if (OwnerASC)
	{
		OwnerASC->AbilityCommittedCallbacks.AddUObject(this, &ThisClass::AbilityCommit);
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
}

void UAbilityGauge::ConfigureWithWidgetData(const FAbilityWidgetData* WidgetData)
{
	if (Icon && WidgetData)
	{
		Icon->GetDynamicMaterial()->SetTextureParameterValue(IconMaterialParamName, WidgetData->Icon.LoadSynchronous());
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
		CooldownTimerHandle, this, &ThisClass::CooldownFinished, CooldownTimeRemaining
	);
	GetWorld()->GetTimerManager().SetTimer(
		CooldownTimerUpdateHandle, this, &ThisClass::UpdateCooldown, CooldownUpdateInterval, true, 0.f
	);
}

void UAbilityGauge::CooldownFinished()
{
	CachedCooldownDuration = 0.f;
	CachedCooldownTimeRemaining = 0.f;
	CooldownCounterText->SetVisibility(ESlateVisibility::Hidden);
	GetWorld()->GetTimerManager().ClearTimer(CooldownTimerUpdateHandle);
	Icon->GetDynamicMaterial()->SetScalarParameterValue(CooldownPercentParamName, 1.f);
}

void UAbilityGauge::UpdateCooldown()
{
	CachedCooldownTimeRemaining -= CooldownUpdateInterval;
	FNumberFormattingOptions* FormattingOptions =
		CachedCooldownTimeRemaining > 1.f ? &WholeNumberFormattingOptions : &OneDigitNumberFormattingOptions;
	CooldownCounterText->SetText(FText::AsNumber(CachedCooldownTimeRemaining, FormattingOptions));
	Icon->GetDynamicMaterial()->SetScalarParameterValue(
		CooldownPercentParamName, 1.f - CachedCooldownTimeRemaining / CachedCooldownDuration
	);
}

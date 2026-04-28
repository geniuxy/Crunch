// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Stats/OverHeadStatsGauge.h"

#include "GAS/CAttributeSet.h"
#include "Widgets/Stats/ValueGauge.h"

void UOverHeadStatsGauge::ConfigureWithASC(UAbilitySystemComponent* AbilitySystemComponent)
{
	if (AbilitySystemComponent)
	{
		HealthBar->SetAndBoundToGameplayAttribute(
			AbilitySystemComponent,
			UCAttributeSet::GetHealthAttribute(),
			UCAttributeSet::GetMaxHealthAttribute()
		);

		ManaBar->SetAndBoundToGameplayAttribute(
			AbilitySystemComponent,
			UCAttributeSet::GetManaAttribute(),
			UCAttributeSet::GetMaxManaAttribute()
		);
	}
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Abilities/AbilityListView.h"

#include "Abilities/GameplayAbility.h"
#include "CTypes/CStruct.h"
#include "Widgets/Abilities/AbilityGauge.h"

void UAbilityListView::ConfigureAbilities(const TMap<ECAbilityInputID, TSubclassOf<UGameplayAbility>>& Abilities)
{
	OnEntryWidgetGenerated().AddUObject(this, &ThisClass::AbilityGaugeGenerated);
	for (TPair<ECAbilityInputID, TSubclassOf<UGameplayAbility>> AbilityPair : Abilities)
	{
		AddItem(AbilityPair.Value.GetDefaultObject());
	}
}

void UAbilityListView::AbilityGaugeGenerated(UUserWidget& Widget)
{
	UAbilityGauge* AbilityGauge = Cast<UAbilityGauge>(&Widget);
	if (AbilityGauge)
	{
		AbilityGauge->ConfigureWithWidgetData(
			FindWidgetDataForAbility(AbilityGauge->GetListItem<UGameplayAbility>()->GetClass())
		);
	}
}

const FAbilityData* UAbilityListView::FindWidgetDataForAbility(
	const TSubclassOf<UGameplayAbility>& AbilityClass) const
{
	if (!AbilityDataTable) return nullptr;

	for (const auto& AbilityDataRowName : AbilityDataTable->GetRowNames())
	{
		FAbilityData* WidgetData = AbilityDataTable->FindRow<FAbilityData>(AbilityDataRowName, "");
		if (WidgetData->AbilityClass == AbilityClass)
		{
			return WidgetData;
		}
	}

	return nullptr;
}

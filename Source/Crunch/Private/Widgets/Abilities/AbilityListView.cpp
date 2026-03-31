// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Abilities/AbilityListView.h"

#include "Abilities/GameplayAbility.h"

void UAbilityListView::ConfigureAbilities(const TMap<ECAbilityInputID, TSubclassOf<UGameplayAbility>>& Abilities)
{
	for (TPair<ECAbilityInputID, TSubclassOf<UGameplayAbility>> AbilityPair : Abilities)
	{
		AddItem(AbilityPair.Value.GetDefaultObject());
	}
}

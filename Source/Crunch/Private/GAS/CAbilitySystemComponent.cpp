// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/CAbilitySystemComponent.h"

void UCAbilitySystemComponent::InitializeBaseAttributes()
{
	if (!GetOwner() || !GetOwner()->HasAuthority()) return;
	
	for (const TSubclassOf<UGameplayEffect>& EffectClass : InitializeGameplayEffects)
	{
		FGameplayEffectSpecHandle SpecHandle = MakeOutgoingSpec(EffectClass, 1, MakeEffectContext());
		ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}
}

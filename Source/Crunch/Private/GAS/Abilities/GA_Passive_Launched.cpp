// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/GA_Passive_Launched.h"

#include "CGameplayTags.h"

UGA_Passive_Launched::UGA_Passive_Launched()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;

	FAbilityTriggerData TriggerData;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	TriggerData.TriggerTag = CGameplayTags::Crunch_Ability_Passive_Launched_Event_Activate;
	AbilityTriggers.Add(TriggerData);
	
	ActivationBlockedTags.RemoveTag(CGameplayTags::Crunch_Stats_Stun);
}

void UGA_Passive_Launched::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	if (!K2_CommitAbility())
	{
		K2_EndAbility();
		return;
	}

	if (K2_HasAuthority())
	{
		PushSelf(TriggerEventData->TargetData.Get(0)->GetHitResult()->ImpactNormal);
		K2_EndAbility();
	}
}

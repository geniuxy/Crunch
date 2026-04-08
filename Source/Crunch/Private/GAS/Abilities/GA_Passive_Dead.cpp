// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/GA_Passive_Dead.h"

#include "CGameplayTags.h"
#include "CrunchDebugHelper.h"

UGA_Passive_Dead::UGA_Passive_Dead()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;

	FAbilityTriggerData TriggerData;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	TriggerData.TriggerTag = CGameplayTags::Crunch_Stats_Dead;
	
	AbilityTriggers.Add(TriggerData);
	
	ActivationBlockedTags.RemoveTag(CGameplayTags::Crunch_Stats_Stun);
}

void UGA_Passive_Dead::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	if (K2_HasAuthority())
	{
		AActor* Killer = TriggerEventData->ContextHandle.GetEffectCauser();
		if (Killer)
		{
			Debug::Print(FString::Printf(TEXT("我被[%s]击杀"), *Killer->GetName()));
		}
	}
}

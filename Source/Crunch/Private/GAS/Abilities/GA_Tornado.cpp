// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/GA_Tornado.h"

#include "CGameplayTags.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitCancel.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"

void UGA_Tornado::ActivateAbility(
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

	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		UAbilityTask_PlayMontageAndWait* PlayTornadoMontage =
			UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, TornadoMontage);
		PlayTornadoMontage->OnBlendOut.AddDynamic(this, &ThisClass::K2_EndAbility);
		PlayTornadoMontage->OnCancelled.AddDynamic(this, &ThisClass::K2_EndAbility);
		PlayTornadoMontage->OnInterrupted.AddDynamic(this, &ThisClass::K2_EndAbility);
		PlayTornadoMontage->OnCompleted.AddDynamic(this, &ThisClass::K2_EndAbility);
		PlayTornadoMontage->ReadyForActivation();

		if (K2_HasAuthority())
		{
			UAbilityTask_WaitGameplayEvent* WaitDamageEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
				this, CGameplayTags::Crunch_Ability_Generic_Event_Damage
			);
			WaitDamageEvent->EventReceived.AddDynamic(this, &ThisClass::TornadoDamageEffectReceived);
			WaitDamageEvent->ReadyForActivation();
		}

		UAbilityTask_WaitCancel* WaitCancel = UAbilityTask_WaitCancel::WaitCancel(this);
		WaitCancel->OnCancel.AddDynamic(this, &ThisClass::K2_EndAbility);
		WaitCancel->ReadyForActivation();

		UAbilityTask_WaitDelay* WaitDelay = UAbilityTask_WaitDelay::WaitDelay(this, TornadoDuration);
		WaitDelay->OnFinish.AddDynamic(this, &ThisClass::K2_EndAbility);
		WaitDelay->ReadyForActivation();
	}
}

void UGA_Tornado::TornadoDamageEffectReceived(FGameplayEventData Payload)
{
	if (K2_HasAuthority())
	{
		FGameplayAbilityTargetDataHandle TargetDataHandle = Payload.TargetData;
		BP_ApplyGameplayEffectToTarget(
			TargetDataHandle, HitDamageEffect, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo)
		);
		PushTargetsFromOwnerLocation(TargetDataHandle, HitPushSpeed);
	}
}

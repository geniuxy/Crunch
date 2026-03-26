// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/GA_UpperCut.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"

void UGA_UpperCut::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	if (K2_CommitAbility())
	{
		K2_EndAbility();
		return;
	}

	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		UAbilityTask_PlayMontageAndWait* PlayerUpperCutMontageTask =
			UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, UpperCutMontage);
		PlayerUpperCutMontageTask->OnBlendOut.AddDynamic(this, &ThisClass::K2_EndAbility);
		PlayerUpperCutMontageTask->OnCancelled.AddDynamic(this, &ThisClass::K2_EndAbility);
		PlayerUpperCutMontageTask->OnInterrupted.AddDynamic(this, &ThisClass::K2_EndAbility);
		PlayerUpperCutMontageTask->OnCompleted.AddDynamic(this, &ThisClass::K2_EndAbility);
		PlayerUpperCutMontageTask->ReadyForActivation();
	}
}

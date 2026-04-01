// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/GA_GroundBlast.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"

void UGA_GroundBlast::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	if (!HasAuthorityOrPredictionKey(GetCurrentActorInfo(), &CurrentActivationInfo)) return;

	UAbilityTask_PlayMontageAndWait* PlayGroundBlastMontageTask =
		UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, GroundBlastMontage);
	PlayGroundBlastMontageTask->OnCompleted.AddDynamic(this, &ThisClass::K2_EndAbility);
	PlayGroundBlastMontageTask->OnBlendOut.AddDynamic(this, &ThisClass::K2_EndAbility);
	PlayGroundBlastMontageTask->OnInterrupted.AddDynamic(this, &ThisClass::K2_EndAbility);
	PlayGroundBlastMontageTask->OnCancelled.AddDynamic(this, &ThisClass::K2_EndAbility);
	PlayGroundBlastMontageTask->ReadyForActivation();
}

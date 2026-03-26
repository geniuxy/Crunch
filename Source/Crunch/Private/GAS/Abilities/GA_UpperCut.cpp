// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/GA_UpperCut.h"

#include "CGameplayTags.h"
#include "CrunchDebugHelper.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"

void UGA_UpperCut::ActivateAbility(
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
		UAbilityTask_PlayMontageAndWait* PlayerUpperCutMontageTask =
			UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, UpperCutMontage);
		PlayerUpperCutMontageTask->OnBlendOut.AddDynamic(this, &ThisClass::K2_EndAbility);
		PlayerUpperCutMontageTask->OnCancelled.AddDynamic(this, &ThisClass::K2_EndAbility);
		PlayerUpperCutMontageTask->OnInterrupted.AddDynamic(this, &ThisClass::K2_EndAbility);
		PlayerUpperCutMontageTask->OnCompleted.AddDynamic(this, &ThisClass::K2_EndAbility);
		PlayerUpperCutMontageTask->ReadyForActivation();
	}
	
	UAbilityTask_WaitGameplayEvent* WaitLaunchingEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this, CGameplayTags::Crunch_Ability_UpperCut_Event_Damage
	);
	WaitLaunchingEventTask->EventReceived.AddDynamic(this, &ThisClass::StartLaunching);
	WaitLaunchingEventTask->ReadyForActivation();
}

void UGA_UpperCut::StartLaunching(FGameplayEventData EventData)
{
	TArray<FHitResult> HitResults = GetHitResultFromSweepLocationTargetData(
		EventData.TargetData, TargetSweepSphereRadius, ETeamAttitude::Hostile, ShouldDrawDebug()
	);

	if (K2_HasAuthority())
	{
		for (FHitResult HitResult : HitResults)
		{
			Debug::Print(FString::Printf(TEXT("I Hit: %s"), *HitResult.GetActor()->GetName()));
		}
	}
}

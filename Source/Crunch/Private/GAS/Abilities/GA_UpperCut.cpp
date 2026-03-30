// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/GA_UpperCut.h"

#include "CGameplayTags.h"
#include "CrunchDebugHelper.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"

UGA_UpperCut::UGA_UpperCut()
{
	BlockAbilitiesWithTag.AddTag(CGameplayTags::Crunch_Ability_BasicAttack);
}

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

		UAbilityTask_WaitGameplayEvent* WaitLaunchingEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
			this, CGameplayTags::Crunch_Ability_UpperCut_Event_Damage
		);
		WaitLaunchingEventTask->EventReceived.AddDynamic(this, &ThisClass::StartLaunching);
		WaitLaunchingEventTask->ReadyForActivation();
	}

	NextComboName = NAME_None;
}

void UGA_UpperCut::StartLaunching(FGameplayEventData EventData)
{
	if (K2_HasAuthority())
	{
		TArray<FHitResult> HitResults = GetHitResultFromSweepLocationTargetData(
			EventData.TargetData, TargetSweepSphereRadius, ETeamAttitude::Hostile, ShouldDrawDebug()
		);

		PushTarget(GetAvatarActorFromActorInfo(), FVector::UpVector * UpperCutLaunchSpeed);
		for (FHitResult HitResult : HitResults)
		{
			PushTarget(HitResult.GetActor(), FVector::UpVector * UpperCutLaunchSpeed);
			ApplyGameplayEffectToHitResultActor(
				HitResult, LaunchDamageEffect, GetAbilityLevel(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo())
			);
		}
	}

	UAbilityTask_WaitGameplayEvent* WaitComboChangeEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this, CGameplayTags::Crunch_Ability_Combo_Event_Change, nullptr, false, false
	);
	WaitComboChangeEvent->EventReceived.AddDynamic(this, &ThisClass::UGA_UpperCut::HandleComboChangeEvent);
	WaitComboChangeEvent->ReadyForActivation();

	UAbilityTask_WaitGameplayEvent* WaitBasicAttackComboCommitEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this, CGameplayTags::Crunch_Ability_BasicAttack_Event_Pressed
	);
	WaitBasicAttackComboCommitEvent->EventReceived.AddDynamic(
		this, &ThisClass::UGA_UpperCut::HandleBasicAttackComboCommitEvent
	);
	WaitBasicAttackComboCommitEvent->ReadyForActivation();

	if (K2_HasAuthority())
	{
		UAbilityTask_WaitGameplayEvent* WaitBasicAttackComboDamageEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
			this, CGameplayTags::Crunch_Ability_BasicAttack_Event_Damage
		);
		WaitBasicAttackComboDamageEvent->EventReceived.AddDynamic(
			this, &ThisClass::UGA_UpperCut::HandleBasicAttackComboDamageEvent
		);
		WaitBasicAttackComboDamageEvent->ReadyForActivation();
	}
}

void UGA_UpperCut::HandleComboChangeEvent(FGameplayEventData EventData)
{
	FGameplayTag EventTag = EventData.EventTag;
	if (EventTag == CGameplayTags::Crunch_Ability_Combo_Event_Change_End)
	{
		NextComboName = NAME_None;
		return;
	}

	TArray<FName> TagNames;
	UGameplayTagsManager::Get().SplitGameplayTagFName(EventTag, TagNames);
	NextComboName = TagNames.Last();
}

void UGA_UpperCut::HandleBasicAttackComboCommitEvent(FGameplayEventData EventData)
{
	if (NextComboName == NAME_None)
	{
		return;
	}

	UAnimInstance* OwnerAnimInstance = GetOwnerAnimInstance();
	if (!OwnerAnimInstance)
	{
		return;
	}

	// woc 天才，这样子就可以不涉及动画之间的过渡，Combo1打完就会自动打Combo2
	OwnerAnimInstance->Montage_SetNextSection(
		OwnerAnimInstance->Montage_GetCurrentSection(UpperCutMontage),
		NextComboName,
		UpperCutMontage
	);
}

void UGA_UpperCut::HandleBasicAttackComboDamageEvent(FGameplayEventData EventData)
{
	if (K2_HasAuthority())
	{
		TArray<FHitResult> HitResults = GetHitResultFromSweepLocationTargetData(
			EventData.TargetData, TargetSweepSphereRadius, ETeamAttitude::Hostile, ShouldDrawDebug()
		);

		PushTarget(GetAvatarActorFromActorInfo(), FVector::UpVector * UpperCutHoldSpeed);
		for (FHitResult HitResult : HitResults)
		{
			PushTarget(HitResult.GetActor(), FVector::UpVector * UpperCutHoldSpeed);
			ApplyGameplayEffectToHitResultActor(
				HitResult, LaunchDamageEffect, GetAbilityLevel(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo())
			);
		}
	}
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/GA_Freeze.h"

#include "AbilitySystemComponent.h"
#include "CGameplayTags.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "FunctionLibrary/CAbilitySystemFunctionLibrary.h"
#include "GAS/TargetActors/TargetActor_GroundPick.h"

UGA_Freeze::UGA_Freeze()
{
	ActivationOwnedTags.AddTag(CGameplayTags::Crunch_Stats_Aim);
}

void UGA_Freeze::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	if (!HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		K2_EndAbility();
		return;
	}

	UAbilityTask_PlayMontageAndWait* PlayTargetMontage =
		UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, FreezeTargetingMontage);
	PlayTargetMontage->OnBlendOut.AddDynamic(this, &ThisClass::K2_EndAbility);
	PlayTargetMontage->OnCancelled.AddDynamic(this, &ThisClass::K2_EndAbility);
	PlayTargetMontage->OnInterrupted.AddDynamic(this, &ThisClass::K2_EndAbility);
	PlayTargetMontage->OnCompleted.AddDynamic(this, &ThisClass::K2_EndAbility);
	PlayTargetMontage->ReadyForActivation();

	UAbilityTask_WaitTargetData* WaitTargetData = UAbilityTask_WaitTargetData::WaitTargetData(
		this, NAME_None, EGameplayTargetingConfirmation::UserConfirmed, TargetActorClass);
	WaitTargetData->ValidData.AddDynamic(this, &ThisClass::TargetReceived);
	WaitTargetData->Cancelled.AddDynamic(this, &ThisClass::TargetCancelled);
	WaitTargetData->ReadyForActivation();

	AGameplayAbilityTargetActor* TargetActor;
	WaitTargetData->BeginSpawningActor(this, TargetActorClass, TargetActor);
	ATargetActor_GroundPick* GroundPickActor = Cast<ATargetActor_GroundPick>(TargetActor);
	if (GroundPickActor)
	{
		GroundPickActor->SetShouldDrawDebug(ShouldDrawDebug());
		GroundPickActor->SetTargetAreaRadius(TargetingAreaRadius);
		GroundPickActor->SetTargetTraceRange(TargetRange);
	}

	WaitTargetData->FinishSpawningActor(this, TargetActor);
}

void UGA_Freeze::TargetReceived(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	if (!K2_CommitAbility())
	{
		K2_EndAbility();
		return;
	}
	
	if (K2_HasAuthority())
	{
		BP_ApplyGameplayEffectToTarget(
			TargetDataHandle, DamageEffect, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo)
		);
	}

	FGameplayCueParameters CueParams;
	CueParams.Location = UCAbilitySystemFunctionLibrary::GetHitResultFromTargetData(TargetDataHandle, 1).ImpactPoint;
	CueParams.RawMagnitude = TargetingAreaRadius;

	GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(FreezingImpactGameplayCueTag, CueParams);

	PlayMontageLocally(FreezeCastMontage);
	K2_EndAbility();
}

void UGA_Freeze::TargetCancelled(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	K2_EndAbility();
}

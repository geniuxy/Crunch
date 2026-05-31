// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/GA_Blink.h"

#include "CGameplayTags.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "FunctionLibrary/CAbilitySystemFunctionLibrary.h"
#include "GAS/TargetActors/TargetActor_GroundPick.h"

UGA_Blink::UGA_Blink()
{
	ActivationOwnedTags.AddTag(CGameplayTags::Crunch_Stats_Aim);
}

void UGA_Blink::ActivateAbility(
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

	UAbilityTask_PlayMontageAndWait* PlayTargetingMontage =
		UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, TargetingMontage);
	PlayTargetingMontage->ReadyForActivation();

	UAbilityTask_WaitTargetData* WaitBlinkLocationTargetData = UAbilityTask_WaitTargetData::WaitTargetData(
		this, NAME_None, EGameplayTargetingConfirmation::UserConfirmed, GroundPickTargetActorClass
	);
	WaitBlinkLocationTargetData->ValidData.AddDynamic(this, &ThisClass::GroundPickTargetReceived);
	WaitBlinkLocationTargetData->Cancelled.AddDynamic(this, &ThisClass::GroundPickCancelled);
	WaitBlinkLocationTargetData->ReadyForActivation();

	AGameplayAbilityTargetActor* TargetActor;
	WaitBlinkLocationTargetData->BeginSpawningActor(this, GroundPickTargetActorClass, TargetActor);
	ATargetActor_GroundPick* GroundPickTargetActor = Cast<ATargetActor_GroundPick>(TargetActor);
	if (GroundPickTargetActor)
	{
		GroundPickTargetActor->SetShouldDrawDebug(ShouldDrawDebug());
		GroundPickTargetActor->SetTargetAreaRadius(TargetAreaRadius);
		GroundPickTargetActor->SetTargetTraceRange(BlinkCastRange);
	}

	WaitBlinkLocationTargetData->FinishSpawningActor(this, TargetActor);
}

void UGA_Blink::GroundPickTargetReceived(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	// CommitAbility放在真正需要计算CD的位置
	if (!K2_CommitAbility())
	{
		K2_EndAbility();
		return;
	}
	
	BlinkTargetDataHandle = TargetDataHandle;

	if (HasAuthorityOrPredictionKey(CurrentActorInfo, &CurrentActivationInfo))
	{
		UAbilityTask_PlayMontageAndWait* PlayTeleportMontage =
			UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, TeleportMontage);
		PlayTeleportMontage->OnBlendOut.AddDynamic(this, &ThisClass::K2_EndAbility);
		PlayTeleportMontage->OnCancelled.AddDynamic(this, &ThisClass::K2_EndAbility);
		PlayTeleportMontage->OnInterrupted.AddDynamic(this, &ThisClass::K2_EndAbility);
		PlayTeleportMontage->OnCompleted.AddDynamic(this, &ThisClass::K2_EndAbility);
		PlayTeleportMontage->ReadyForActivation();

		if (K2_HasAuthority())
		{
			UAbilityTask_WaitGameplayEvent* WaitTeleportTimePoint = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
				this, CGameplayTags::Crunch_Ability_Blink_Event_Teleport
			);
			WaitTeleportTimePoint->EventReceived.AddDynamic(this, &ThisClass::Teleport);
			WaitTeleportTimePoint->ReadyForActivation();
		}
	}
}

void UGA_Blink::GroundPickCancelled(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	K2_EndAbility();
}

void UGA_Blink::Teleport(FGameplayEventData Payload)
{
	if (K2_HasAuthority())
	{
		FHitResult PickedLocationHitResult =
			UCAbilitySystemFunctionLibrary::GetHitResultFromTargetData(BlinkTargetDataHandle, 1);
		FVector PickedTeleportLocation = PickedLocationHitResult.ImpactPoint;

		GetAvatarActorFromActorInfo()->SetActorLocation(PickedTeleportLocation);
		BP_ApplyGameplayEffectToTarget(
			BlinkTargetDataHandle, DamageEffect, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo)
		);
		PushTargetsFromOwnerLocation(BlinkTargetDataHandle, BlinkLandTargetPushSpeed);
	}
}

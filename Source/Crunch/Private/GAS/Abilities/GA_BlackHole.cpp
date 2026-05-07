// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/GA_BlackHole.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "CGameplayTags.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "GAS/TargetActors/TargetActor_GroundPick.h"
#include "GAS/TargetActors/TargetActor_BlackHole.h"

void UGA_BlackHole::ActivateAbility(
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

	PlayCastBlackHoleMontageTask =
		UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, TargetingMontage);
	PlayCastBlackHoleMontageTask->OnBlendOut.AddDynamic(this, &ThisClass::K2_EndAbility);
	PlayCastBlackHoleMontageTask->OnCancelled.AddDynamic(this, &ThisClass::K2_EndAbility);
	PlayCastBlackHoleMontageTask->OnInterrupted.AddDynamic(this, &ThisClass::K2_EndAbility);
	PlayCastBlackHoleMontageTask->OnCompleted.AddDynamic(this, &ThisClass::K2_EndAbility);
	PlayCastBlackHoleMontageTask->ReadyForActivation();

	UAbilityTask_WaitTargetData* WaitPlacementTask = UAbilityTask_WaitTargetData::WaitTargetData(
		this, NAME_None, EGameplayTargetingConfirmation::UserConfirmed, TargetActorClass
	);
	WaitPlacementTask->ValidData.AddDynamic(this, &ThisClass::PlaceBlackHole);
	WaitPlacementTask->Cancelled.AddDynamic(this, &ThisClass::PlacementCancelled);
	WaitPlacementTask->ReadyForActivation();

	AGameplayAbilityTargetActor* TargetActor;
	WaitPlacementTask->BeginSpawningActor(this, TargetActorClass, TargetActor);

	ATargetActor_GroundPick* GroundPickTargetActor = Cast<ATargetActor_GroundPick>(TargetActor);
	if (GroundPickTargetActor)
	{
		GroundPickTargetActor->SetShouldDrawDebug(ShouldDrawDebug());
		GroundPickTargetActor->SetTargetAreaRadius(TargetAreaRadius);
		GroundPickTargetActor->SetTargetTraceRange(TargetTraceRange);
	}

	WaitPlacementTask->FinishSpawningActor(this, TargetActor);
	AddAimEffect();
}

void UGA_BlackHole::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled)
{
	RemoveAimEffect();
	RemoveFocusEffect();

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_BlackHole::AddAimEffect()
{
	AimEffectHandle = BP_ApplyGameplayEffectToOwner(AimEffect);
}

void UGA_BlackHole::RemoveAimEffect()
{
	if (AimEffectHandle.IsValid())
	{
		BP_RemoveGameplayEffectFromOwnerWithHandle(AimEffectHandle);
	}
}

void UGA_BlackHole::AddFocusEffect()
{
	FocusEffectHandle = BP_ApplyGameplayEffectToOwner(FocusEffect);
}

void UGA_BlackHole::RemoveFocusEffect()
{
	if (FocusEffectHandle.IsValid())
	{
		BP_RemoveGameplayEffectFromOwnerWithHandle(FocusEffectHandle);
	}
}

void UGA_BlackHole::PlaceBlackHole(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	if (!K2_CommitAbility())
	{
		K2_EndAbility();
		return;
	}

	RemoveAimEffect();
	AddFocusEffect();

	if (PlayCastBlackHoleMontageTask)
	{
		PlayCastBlackHoleMontageTask->OnBlendOut.RemoveAll(this);
		PlayCastBlackHoleMontageTask->OnCancelled.RemoveAll(this);
		PlayCastBlackHoleMontageTask->OnInterrupted.RemoveAll(this);
		PlayCastBlackHoleMontageTask->OnCompleted.RemoveAll(this);
	}

	if (HasAuthorityOrPredictionKey(CurrentActorInfo, &CurrentActivationInfo))
	{
		UAbilityTask_PlayMontageAndWait* PlayHoldBlackHoleMontage =
			UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, HoldBlackHoleMontage);
		PlayHoldBlackHoleMontage->OnBlendOut.AddDynamic(this, &ThisClass::K2_EndAbility);
		PlayHoldBlackHoleMontage->OnCancelled.AddDynamic(this, &ThisClass::K2_EndAbility);
		PlayHoldBlackHoleMontage->OnInterrupted.AddDynamic(this, &ThisClass::K2_EndAbility);
		PlayHoldBlackHoleMontage->OnCompleted.AddDynamic(this, &ThisClass::K2_EndAbility);
		PlayHoldBlackHoleMontage->ReadyForActivation();
	}

	BlackHoleTargetingTask = UAbilityTask_WaitTargetData::WaitTargetData(
		this, NAME_None, EGameplayTargetingConfirmation::UserConfirmed, BlackHoleTargetActorClass
	);
	BlackHoleTargetingTask->ValidData.AddDynamic(this, &ThisClass::FinalTargetsReceived);
	BlackHoleTargetingTask->Cancelled.AddDynamic(this, &ThisClass::FinalTargetsReceived);
	BlackHoleTargetingTask->ReadyForActivation();

	AGameplayAbilityTargetActor* TargetActor;
	BlackHoleTargetingTask->BeginSpawningActor(this, BlackHoleTargetActorClass, TargetActor);
	ATargetActor_BlackHole* BlackHoleTargetActor = Cast<ATargetActor_BlackHole>(TargetActor);
	if (BlackHoleTargetActor)
	{
		BlackHoleTargetActor->ConfigureBlackHole(
			TargetAreaRadius, BlackHolePullSpeed, BlackHoleDuration, GetOwnerTeamID()
		);
	}

	BlackHoleTargetingTask->FinishSpawningActor(this, TargetActor);
	if (BlackHoleTargetingTask)
	{
		BlackHoleTargetActor->SetActorLocation(
			UAbilitySystemBlueprintLibrary::GetHitResultFromTargetData(TargetDataHandle, 1).ImpactPoint
		);
		// TargetActorTask的FinishSpawningActor位置默认在Owner的位置
		// 所以需要在SetActorLocation之后设置黑洞Link中心的位置
		BlackHoleTargetActor->SetBlackHoleLinkOrigin();
	}
}

void UGA_BlackHole::PlacementCancelled(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	K2_EndAbility();
}

void UGA_BlackHole::FinalTargetsReceived(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	if (K2_HasAuthority())
	{
		BP_ApplyGameplayEffectToTarget(
			TargetDataHandle, FinalBlowDamageEffect, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo)
		);
		FVector BlowCenter =
			UAbilitySystemBlueprintLibrary::GetHitResultFromTargetData(TargetDataHandle, 1).ImpactPoint;
		PushTargetsFromLocation(TargetDataHandle, BlowCenter, BlowPushSpeed);

		UAbilityTask_PlayMontageAndWait* PlayFinalBlowMontage =
			UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, FinalBlowMontage);
		PlayFinalBlowMontage->OnBlendOut.AddDynamic(this, &ThisClass::K2_EndAbility);
		PlayFinalBlowMontage->OnCancelled.AddDynamic(this, &ThisClass::K2_EndAbility);
		PlayFinalBlowMontage->OnInterrupted.AddDynamic(this, &ThisClass::K2_EndAbility);
		PlayFinalBlowMontage->OnCompleted.AddDynamic(this, &ThisClass::K2_EndAbility);
		PlayFinalBlowMontage->ReadyForActivation();
	}
	else
	{
		PlayMontageLocally(FinalBlowMontage);
	}

	FGameplayCueParameters FinalBlowCueParameters;
	FinalBlowCueParameters.Location =
		UAbilitySystemBlueprintLibrary::GetHitResultFromTargetData(TargetDataHandle, 1).ImpactPoint;
	FinalBlowCueParameters.RawMagnitude = TargetAreaRadius;

	GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(FinalBlowCueTag, FinalBlowCueParameters);
	GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(CGameplayTags::GameplayCue_CameraShake);
}

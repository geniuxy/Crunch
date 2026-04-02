// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/GA_GroundBlast.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "CGameplayTags.h"
#include "CrunchDebugHelper.h"
#include "GAS/CAbilitySystemComponent.h"
#include "GAS/TargetActors/TargetActor_GroundPick.h"

UGA_GroundBlast::UGA_GroundBlast()
{
	ActivationOwnedTags.AddTag(CGameplayTags::Crunch_Stats_Aim);
	BlockAbilitiesWithTag.AddTag(CGameplayTags::Crunch_Ability_BasicAttack);
	ActivationBlockedTags.AddTag(CGameplayTags::Crunch_Stats_DisableAim);
}

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

	UAbilityTask_WaitTargetData* WaitTargetDataTask = UAbilityTask_WaitTargetData::WaitTargetData(
		this, NAME_None, EGameplayTargetingConfirmation::UserConfirmed, TargetActorClass
	);
	WaitTargetDataTask->ValidData.AddDynamic(this, &ThisClass::TargetConfirmed);
	WaitTargetDataTask->Cancelled.AddDynamic(this, &ThisClass::TargetCancelled);
	WaitTargetDataTask->ReadyForActivation();

	AGameplayAbilityTargetActor* TargetActor;
	WaitTargetDataTask->BeginSpawningActor(this, TargetActorClass, TargetActor);

	ATargetActor_GroundPick* GroundPickActor = Cast<ATargetActor_GroundPick>(TargetActor);
	if (GroundPickActor)
	{
		GroundPickActor->SetShouldDrawDebug(ShouldDrawDebug());
		GroundPickActor->SetTargetAreaRadius(TargetAreaRadius);
		GroundPickActor->SetTargetTraceRange(TargetTraceRange);
	}
	
	WaitTargetDataTask->FinishSpawningActor(this, TargetActor);
}

void UGA_GroundBlast::TargetConfirmed(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	BP_ApplyGameplayEffectToTarget(
		TargetDataHandle, DamageEffectDef.DamageEffect, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo)
	);

	Debug::Print(TEXT("Target Confirmed"));
	UCAbilitySystemComponent* OwnerASC = Cast<UCAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo());
	if (OwnerASC)
	{
		OwnerASC->DisableAim();
	}
	K2_EndAbility();
}

void UGA_GroundBlast::TargetCancelled(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	Debug::Print(TEXT("Target Canceled"));
	
	UCAbilitySystemComponent* OwnerASC = Cast<UCAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo());
	if (OwnerASC)
	{
		OwnerASC->DisableAim();
	}
	K2_EndAbility();
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/GA_Dash.h"

#include "AbilitySystemComponent.h"
#include "CGameplayTags.h"
#include "GAS/TargetActors/TargetActor_Around.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "GameFramework/CharacterMovementComponent.h"

void UGA_Dash::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	if (!K2_CommitAbility() || !DashMontage)
	{
		K2_EndAbility();
		return;
	}

	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		UAbilityTask_PlayMontageAndWait* PlayDashMontage =
			UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, DashMontage);
		PlayDashMontage->OnBlendOut.AddDynamic(this, &ThisClass::K2_EndAbility);
		PlayDashMontage->OnCancelled.AddDynamic(this, &ThisClass::K2_EndAbility);
		PlayDashMontage->OnInterrupted.AddDynamic(this, &ThisClass::K2_EndAbility);
		PlayDashMontage->OnCompleted.AddDynamic(this, &ThisClass::K2_EndAbility);
		PlayDashMontage->ReadyForActivation();

		UAbilityTask_WaitGameplayEvent* WaitDashStartEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
			this, CGameplayTags::Crunch_Ability_Dash_Event_Start
		);
		WaitDashStartEvent->EventReceived.AddDynamic(this, &ThisClass::StartDash);
		WaitDashStartEvent->ReadyForActivation();
	}
}

void UGA_Dash::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled)
{
	UAbilitySystemComponent* OwnerAbilitySystemComponent = GetAbilitySystemComponentFromActorInfo();
	if (OwnerAbilitySystemComponent && DashEffectHandle.IsValid())
	{
		OwnerAbilitySystemComponent->RemoveActiveGameplayEffect(DashEffectHandle);
	}

	if (PushForwardInputTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(PushForwardInputTimerHandle);
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_Dash::PushForward()
{
	if (OwnerCharacterMovementComponent)
	{
		FVector ForwardVector = GetAvatarActorFromActorInfo()->GetActorForwardVector();
		OwnerCharacterMovementComponent->AddInputVector(ForwardVector);
		PushForwardInputTimerHandle = GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::PushForward);
	}
}

void UGA_Dash::StartDash(FGameplayEventData Payload)
{
	if (K2_HasAuthority())
	{
		if (DashEffect)
		{
			DashEffectHandle =
				BP_ApplyGameplayEffectToOwner(DashEffect, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
		}
	}

	if (IsLocallyControlled())
	{
		PushForwardInputTimerHandle = GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::PushForward);
		OwnerCharacterMovementComponent =
			GetAvatarActorFromActorInfo()->GetComponentByClass<UCharacterMovementComponent>();
	}

	UAbilityTask_WaitTargetData* WaitTargetData = UAbilityTask_WaitTargetData::WaitTargetData(
		this, NAME_None, EGameplayTargetingConfirmation::CustomMulti, TargetActorClass
	);
	WaitTargetData->ValidData.AddDynamic(this, &ThisClass::TargetReceived);
	WaitTargetData->ReadyForActivation();

	AGameplayAbilityTargetActor* TargetActor;
	WaitTargetData->BeginSpawningActor(this, TargetActorClass, TargetActor);
	ATargetActor_Around* TargetActorAround = Cast<ATargetActor_Around>(TargetActor);
	if (TargetActorAround)
	{
		TargetActorAround->ConfigureDetection(TargetDetectionRadius, GetOwnerTeamID(), LocalGameplayCueTag);
	}

	WaitTargetData->FinishSpawningActor(this, TargetActor);
	if (TargetActorAround)
	{
		TargetActorAround->AttachToComponent(
			GetOwningComponentFromActorInfo(),
			FAttachmentTransformRules::SnapToTargetNotIncludingScale,
			TargetActorAttachSocketName
		);
	}
}

void UGA_Dash::TargetReceived(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	if (K2_HasAuthority())
	{
		BP_ApplyGameplayEffectToTarget(
			TargetDataHandle, DamageEffect, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo)
		);
		PushTargetsFromOwnerLocation(TargetDataHandle, TargetHitPushSpeed);
	}
}

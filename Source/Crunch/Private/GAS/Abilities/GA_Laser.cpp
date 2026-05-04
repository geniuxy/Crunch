// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/GA_Laser.h"

#include "AbilitySystemComponent.h"
#include "CGameplayTags.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitCancel.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "GAS/TargetActors/TargetActor_Line.h"
#include "GAS/CAttributeSet.h"

void UGA_Laser::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	if (!K2_CommitAbility() || !LaserMontage)
	{
		K2_EndAbility();
		return;
	}

	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		UAbilityTask_PlayMontageAndWait* PlayLaserMontageTask =
			UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, LaserMontage);
		PlayLaserMontageTask->OnBlendOut.AddDynamic(this, &ThisClass::K2_EndAbility);
		PlayLaserMontageTask->OnCancelled.AddDynamic(this, &ThisClass::K2_EndAbility);
		PlayLaserMontageTask->OnInterrupted.AddDynamic(this, &ThisClass::K2_EndAbility);
		PlayLaserMontageTask->OnCompleted.AddDynamic(this, &ThisClass::K2_EndAbility);
		PlayLaserMontageTask->ReadyForActivation();

		UAbilityTask_WaitGameplayEvent* WaitLaserEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
			this, CGameplayTags::Crunch_Ability_Laser_Event_Shoot
		);
		WaitLaserEvent->EventReceived.AddDynamic(this, &ThisClass::ShootLaser);
		WaitLaserEvent->ReadyForActivation();

		UAbilityTask_WaitCancel* WaitCancel = UAbilityTask_WaitCancel::WaitCancel(this);
		WaitCancel->OnCancel.AddDynamic(this, &ThisClass::K2_EndAbility);
		WaitCancel->ReadyForActivation();
	}
}

void UGA_Laser::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled)
{
	UAbilitySystemComponent* OwnerAbilitySystemComponent = GetAbilitySystemComponentFromActorInfo();
	if (OwnerAbilitySystemComponent && OnGoingConsumptionEffectHandle.IsValid())
	{
		OwnerAbilitySystemComponent->RemoveActiveGameplayEffect(OnGoingConsumptionEffectHandle);
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_Laser::ShootLaser(FGameplayEventData Payload)
{
	if (K2_HasAuthority())
	{
		OnGoingConsumptionEffectHandle = BP_ApplyGameplayEffectToOwner(
			OnGoingConsumptionEffect, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo)
		);
		if (UAbilitySystemComponent* OwnerAbilitySystemComponent = GetAbilitySystemComponentFromActorInfo())
		{
			OwnerAbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
				UCAttributeSet::GetManaAttribute()).AddUObject(this, &ThisClass::ManaUpdated);
		}
	}

	UAbilityTask_WaitTargetData* WaitDamageTargetData = UAbilityTask_WaitTargetData::WaitTargetData(
		this, NAME_None, EGameplayTargetingConfirmation::CustomMulti, LaserTargetActorClass
	);
	WaitDamageTargetData->ValidData.AddDynamic(this, &ThisClass::TargetActorReceived);
	WaitDamageTargetData->ReadyForActivation();

	AGameplayAbilityTargetActor* TargetActor;
	WaitDamageTargetData->BeginSpawningActor(this, LaserTargetActorClass, TargetActor);
	WaitDamageTargetData->FinishSpawningActor(this, TargetActor);

	ATargetActor_Line* LineTargetActor = Cast<ATargetActor_Line>(TargetActor);
	if (LineTargetActor)
	{
		LineTargetActor->AttachToComponent(
			GetOwningComponentFromActorInfo(),
			FAttachmentTransformRules::SnapToTargetNotIncludingScale,
			TargetActorAttachSocketName
		);
	}
}

void UGA_Laser::ManaUpdated(const FOnAttributeChangeData& ChangeData)
{
	UAbilitySystemComponent* OwnerAbilitySystemComponent = GetAbilitySystemComponentFromActorInfo();
	if (OwnerAbilitySystemComponent && !OwnerAbilitySystemComponent->CanApplyAttributeModifiers(
			OnGoingConsumptionEffect.GetDefaultObject(),
			GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo),
			MakeEffectContext(CurrentSpecHandle, CurrentActorInfo)
		)
	)
	{
		K2_EndAbility();
	}
}

void UGA_Laser::TargetActorReceived(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
}

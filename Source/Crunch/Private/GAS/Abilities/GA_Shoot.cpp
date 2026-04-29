// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/GA_Shoot.h"

#include "CGameplayTags.h"
#include "CrunchDebugHelper.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"

UGA_Shoot::UGA_Shoot()
{
	ActivationOwnedTags.AddTag(CGameplayTags::Crunch_Stats_Aim);
	ActivationBlockedTags.AddTag(CGameplayTags::Crunch_Stats_DisableAim);
}

void UGA_Shoot::ActivateAbility(
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

	Debug::Print(TEXT("开始射击Ability"));

	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		UAbilityTask_WaitGameplayEvent* WaitStartShootingEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
			this, CGameplayTags::Crunch_Ability_BasicAttack_Event_Pressed
		);
		WaitStartShootingEvent->EventReceived.AddDynamic(this, &ThisClass::StartShooting);
		WaitStartShootingEvent->ReadyForActivation();

		UAbilityTask_WaitGameplayEvent* WaitStopShootingEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
			this, CGameplayTags::Crunch_Ability_BasicAttack_Event_Released
		);
		WaitStopShootingEvent->EventReceived.AddDynamic(this, &ThisClass::StopShooting);
		WaitStopShootingEvent->ReadyForActivation();

		UAbilityTask_WaitGameplayEvent* WaitShootProjectileEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
			this, CGameplayTags::Crunch_Ability_Shoot, nullptr, false, false
		);
		WaitShootProjectileEvent->EventReceived.AddDynamic(this, &ThisClass::ShootProjectile);
		WaitShootProjectileEvent->ReadyForActivation();
	}
}

void UGA_Shoot::InputReleased(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo)
{
	Debug::Print(TEXT("结束射击Ability"));

	K2_EndAbility();
}

void UGA_Shoot::StartShooting(FGameplayEventData Payload)
{
	Debug::Print(TEXT("开始射击！"));
}

void UGA_Shoot::StopShooting(FGameplayEventData Payload)
{
	Debug::Print(TEXT("停止射击！"));
}

void UGA_Shoot::ShootProjectile(FGameplayEventData Payload)
{
	Debug::Print(TEXT("生成子弹！"));
}

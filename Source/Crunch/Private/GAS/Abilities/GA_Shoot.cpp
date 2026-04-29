// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/GA_Shoot.h"

#include "CGameplayTags.h"
#include "CrunchDebugHelper.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_NetworkSyncPoint.h"

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
	// 这里用HasAuthorityOrPredictionKey会报错
	// 这是因为PredictionKey仅在ActivateAbility函数时有效，过期就失效了，详细搜索PredictionKeyWhenActivated
	// if (HasAuthorityOrPredictionKey(CurrentActorInfo, &CurrentActivationInfo))
	// 解决方法：1. 本地和服务端分别播放Montage（不考虑客户端预测） 2. AbilityTask_NetworkSyncPoint（估计是Server等待Client以同步）（考虑客户端预测）
	// UAbilityTask_NetworkSyncPoint* NetWorkSync = UAbilityTask_NetworkSyncPoint::WaitNetSync(this, EAbilityTaskNetSyncType::OnlyServerWait);
	// NetWorkSync->ReadyForActivation();

	if (HasAuthority(&CurrentActivationInfo))
	{
		UAbilityTask_PlayMontageAndWait* PlayShootMontage =
			UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, ShootMontage);
		PlayShootMontage->ReadyForActivation();
	}
	else
	{
		PlayMontageLocally(ShootMontage);
	}
}

void UGA_Shoot::StopShooting(FGameplayEventData Payload)
{
	Debug::Print(TEXT("停止射击！"));
	if (ShootMontage)
	{
		StopMontageAfterCurrentSection(ShootMontage);
	}
}

void UGA_Shoot::ShootProjectile(FGameplayEventData Payload)
{
	Debug::Print(TEXT("生成子弹！"));
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/GA_Shoot.h"

#include "AbilitySystemComponent.h"
#include "CGameplayTags.h"
#include "CrunchDebugHelper.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Actors/Projectiles/ProjectileActor.h"
#include "FunctionLibrary/CAbilitySystemFunctionLibrary.h"

UGA_Shoot::UGA_Shoot()
{
	ActivationOwnedTags.AddTag(CGameplayTags::Crunch_Stats_Aim);
	ActivationOwnedTags.AddTag(CGameplayTags::Crunch_Stats_Crosshair);
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

void UGA_Shoot::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled)
{
	if (AimTargetAbilitySystemComponent)
	{
		AimTargetAbilitySystemComponent->RegisterGameplayTagEvent(CGameplayTags::Crunch_Stats_Dead).RemoveAll(this);
		AimTargetAbilitySystemComponent = nullptr;
	}

	StopShooting(FGameplayEventData());
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
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

	FindAimTarget();
	StartAimTargetCheckTimer();
}

void UGA_Shoot::StopShooting(FGameplayEventData Payload)
{
	Debug::Print(TEXT("停止射击！"));
	if (ShootMontage)
	{
		StopMontageAfterCurrentSection(ShootMontage);
	}

	StopAimTargetCheckTimer();
}

void UGA_Shoot::ShootProjectile(FGameplayEventData Payload)
{
	Debug::Print(TEXT("生成子弹！"));

	if (K2_HasAuthority())
	{
		AActor* OwnerActor = GetAvatarActorFromActorInfo();
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = OwnerActor;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		FVector SocketLocation = OwnerActor->GetActorLocation();
		USkeletalMeshComponent* MeshComp = GetOwningComponentFromActorInfo(); // 这个方法竟然是用来获取SkeletalComponent的
		if (MeshComp)
		{
			TArray<FName> OutNames;
			UGameplayTagsManager::Get().SplitGameplayTagFName(Payload.EventTag, OutNames);
			if (OutNames.Num() != 0)
			{
				FName SocketName = OutNames.Last();
				SocketLocation = MeshComp->GetSocketLocation(SocketName);
			}
		}

		AProjectileActor* Projectile = GetWorld()->SpawnActor<AProjectileActor>(
			ProjectileClass, SocketLocation, OwnerActor->GetActorRotation(), SpawnParams
		);
		if (Projectile)
		{
			Projectile->ShootProjectile(
				ShootProjectileSpeed,
				ShootProjectileRange,
				GetAimTargetIfValid(),
				GetOwnerTeamID(),
				MakeOutgoingGameplayEffectSpec(
					ProjectileHitEffect, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo)
				)
			);
		}
	}
}

AActor* UGA_Shoot::GetAimTargetIfValid() const
{
	if (HasValidTarget())
	{
		return AimTarget;
	}
	return nullptr;
}

void UGA_Shoot::FindAimTarget()
{
	if (HasValidTarget()) return;

	if (AimTargetAbilitySystemComponent)
	{
		AimTargetAbilitySystemComponent->RegisterGameplayTagEvent(CGameplayTags::Crunch_Stats_Dead).RemoveAll(this);
		AimTargetAbilitySystemComponent = nullptr;
	}

	AimTarget = GetAimTarget(ShootProjectileRange, ETeamAttitude::Hostile);
	if (AimTarget)
	{
		AimTargetAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(AimTarget);
		if (AimTargetAbilitySystemComponent)
		{
			AimTargetAbilitySystemComponent->RegisterGameplayTagEvent(CGameplayTags::Crunch_Stats_Dead).AddUObject(
				this, &ThisClass::TargetDeadTagUpdated
			);
		}
	}
}

void UGA_Shoot::StartAimTargetCheckTimer()
{
	UWorld* World = GetWorld();
	if (World)
	{
		World->GetTimerManager().SetTimer(
			AimTargetCheckTimerHandle, this, &UGA_Shoot::FindAimTarget, AimTargetCheckTimeInterval, true
		);
	}
}

void UGA_Shoot::StopAimTargetCheckTimer()
{
	UWorld* World = GetWorld();
	if (World)
	{
		World->GetTimerManager().ClearTimer(AimTargetCheckTimerHandle);
	}
}

bool UGA_Shoot::HasValidTarget() const
{
	if (!AimTarget) return false;
	if (UCAbilitySystemFunctionLibrary::IsActorDead(AimTarget)) return false;
	if (!IsTargetInRange()) return false;

	return true;
}

bool UGA_Shoot::IsTargetInRange() const
{
	if (!AimTarget) return false;

	float Distance = FVector::Distance(
		AimTarget->GetActorLocation(), GetAvatarActorFromActorInfo()->GetActorLocation()
	);
	return Distance <= ShootProjectileRange;
}

void UGA_Shoot::TargetDeadTagUpdated(const FGameplayTag Tag, int32 NewCount)
{
	if (NewCount > 0)
	{
		FindAimTarget();
	}
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/GA_Combo.h"

#include "CGameplayTags.h"
#include "CrunchDebugHelper.h"
#include "Abilities/GameplayAbility.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"

UGA_Combo::UGA_Combo()
{
	// AbilityTags.AddTag(CGameplayTags::Crunch_Ability_BasicAttack);
	// 设置AbilityTags
	AbilityTags.AddTag(CGameplayTags::Crunch_Ability_BasicAttack);
	BlockAbilitiesWithTag.AddTag(CGameplayTags::Crunch_Ability_BasicAttack);
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGA_Combo::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// 用K2_的简化蓝图版本接口，可以简化代码、同时也会调用底层c++方法
	if (!K2_CommitAbility())
	{
		K2_EndAbility();
		return;
	}

	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		UAbilityTask_PlayMontageAndWait* PlayComboMontageTask =
			UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, ComboMontage);
		PlayComboMontageTask->OnBlendOut.AddDynamic(this, &ThisClass::K2_EndAbility);
		PlayComboMontageTask->OnCancelled.AddDynamic(this, &ThisClass::K2_EndAbility);
		PlayComboMontageTask->OnCompleted.AddDynamic(this, &ThisClass::K2_EndAbility);
		PlayComboMontageTask->OnInterrupted.AddDynamic(this, &ThisClass::K2_EndAbility);
		PlayComboMontageTask->ReadyForActivation();

		UAbilityTask_WaitGameplayEvent* WaitGameplayEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
			this, CGameplayTags::Crunch_Ability_BasicAttack_Event_Change, nullptr, false, false
		);
		WaitGameplayEventTask->EventReceived.AddDynamic(this, &ThisClass::ComboChangedEventReceived);
		WaitGameplayEventTask->ReadyForActivation();
	}

	if (K2_HasAuthority())
	{
		UAbilityTask_WaitGameplayEvent* WaitTargetingEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
			this, CGameplayTags::Crunch_Ability_BasicAttack_Event_Damage
		);
		WaitTargetingEventTask->EventReceived.AddDynamic(this, &ThisClass::DoDamage);
		WaitTargetingEventTask->ReadyForActivation();
	}

	SetupWaitComboInputPress();
}

void UGA_Combo::SetupWaitComboInputPress()
{
	UAbilityTask_WaitInputPress* WaitInputPress = UAbilityTask_WaitInputPress::WaitInputPress(this);
	WaitInputPress->OnPress.AddDynamic(this, &ThisClass::HandleInputPress);
	WaitInputPress->ReadyForActivation();
}

void UGA_Combo::HandleInputPress(float TimeWaited)
{
	SetupWaitComboInputPress();
	TryCommitCombo();
}

void UGA_Combo::TryCommitCombo()
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
		OwnerAnimInstance->Montage_GetCurrentSection(ComboMontage),
		NextComboName,
		ComboMontage
	);
}

void UGA_Combo::ComboChangedEventReceived(FGameplayEventData Data)
{
	FGameplayTag EventTag = Data.EventTag;

	if (EventTag == CGameplayTags::Crunch_Ability_BasicAttack_Event_Change_End)
	{
		NextComboName = NAME_None;
		Debug::Print(TEXT("Next Combo Name清空了"));
		return;
	}

	TArray<FName> TagNames;
	UGameplayTagsManager::Get().SplitGameplayTagFName(EventTag, TagNames);
	NextComboName = TagNames.Last();
	Debug::Print(FString::Printf(TEXT("Next Combo Name:%s"), *NextComboName.ToString()));
}

void UGA_Combo::DoDamage(FGameplayEventData Data)
{
	TArray<FHitResult> HitResults = GetHitResultFromSweepLocationTargetData(Data.TargetData, 30.f, true);
}

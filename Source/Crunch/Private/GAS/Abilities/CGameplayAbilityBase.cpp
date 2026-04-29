// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/CGameplayAbilityBase.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "CGameplayTags.h"
#include "CrunchDebugHelper.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"

UCGameplayAbilityBase::UCGameplayAbilityBase()
{
	ActivationBlockedTags.AddTag(CGameplayTags::Crunch_Stats_Stun);
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

bool UCGameplayAbilityBase::CanActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags,
	FGameplayTagContainer* OptionalRelevantTags) const
{
	FGameplayAbilitySpec* AbilitySpec = ActorInfo->AbilitySystemComponent->FindAbilitySpecFromHandle(Handle);
	if (AbilitySpec && AbilitySpec->Level <= 0)
	{
		return false;
	}
	
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

UAnimInstance* UCGameplayAbilityBase::GetOwnerAnimInstance() const
{
	USkeletalMeshComponent* OwnerSkeletalMeshComp = GetOwningComponentFromActorInfo();
	if (OwnerSkeletalMeshComp)
	{
		return OwnerSkeletalMeshComp->GetAnimInstance();
	}
	return nullptr;
}

TArray<FHitResult> UCGameplayAbilityBase::GetHitResultFromSweepLocationTargetData(
	const FGameplayAbilityTargetDataHandle& TargetDataHandle,
	float SphereSweepRadius,
	ETeamAttitude::Type TargetTeam,
	bool bDrawDebug,
	bool bIgnoreSelf) const
{
	TArray<FHitResult> OutResults;
	TSet<AActor*> HitActors;

	IGenericTeamAgentInterface* OwnerTeamInterface = Cast<IGenericTeamAgentInterface>(GetAvatarActorFromActorInfo());

	for (const TSharedPtr<FGameplayAbilityTargetData> TargetData : TargetDataHandle.Data)
	{
		FVector StartLoc = TargetData->GetOrigin().GetTranslation();
		FVector EndLoc = TargetData->GetEndPoint();

		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

		TArray<AActor*> ActorsToIgnore;
		if (bIgnoreSelf)
		{
			ActorsToIgnore.Add(GetAvatarActorFromActorInfo());
		}

		EDrawDebugTrace::Type DrawDebugTrace = bDrawDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;

		TArray<FHitResult> SweepResults;
		UKismetSystemLibrary::SphereTraceMultiForObjects(
			this, StartLoc, EndLoc, SphereSweepRadius, ObjectTypes, false,
			ActorsToIgnore, DrawDebugTrace, SweepResults, false);

		for (const FHitResult& SweepResult : SweepResults)
		{
			if (HitActors.Contains(SweepResult.GetActor())) continue;

			if (OwnerTeamInterface)
			{
				ETeamAttitude::Type OtherActorTeamAttribute =
					OwnerTeamInterface->GetTeamAttitudeTowards(*SweepResult.GetActor());
				if (OtherActorTeamAttribute != TargetTeam)
				{
					continue;
				}
			}

			HitActors.Add(SweepResult.GetActor());
			OutResults.Add(SweepResult);
		}
	}

	return OutResults;
}

void UCGameplayAbilityBase::PushSelf(const FVector& PushVel)
{
	ACharacter* AvatarCharacter = GetOwnerAvatarCharacter();
	if (AvatarCharacter)
	{
		AvatarCharacter->LaunchCharacter(PushVel, true, true);
	}
}

void UCGameplayAbilityBase::PushTarget(AActor* Target, const FVector& PushVel)
{
	if (!Target) return;

	FGameplayEventData EventData;
	FGameplayAbilityTargetData_SingleTargetHit* HitData = new FGameplayAbilityTargetData_SingleTargetHit;
	FHitResult HitResult;
	HitResult.ImpactNormal = PushVel;
	HitData->HitResult = HitResult;
	EventData.TargetData.Add(HitData);

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
		Target, CGameplayTags::Crunch_Ability_Passive_Launched_Event_Activate, EventData
	);
}

void UCGameplayAbilityBase::PushTargets(const TArray<AActor*>& Targets, const FVector& PushVel)
{
	for (AActor* Target : Targets)
	{
		PushTarget(Target, PushVel);
	}
}

void UCGameplayAbilityBase::PushTargets(
	const FGameplayAbilityTargetDataHandle& TargetDataHandle, const FVector& PushVel)
{
	TArray<AActor*> Targets = UAbilitySystemBlueprintLibrary::GetAllActorsFromTargetData(TargetDataHandle);
	PushTargets(Targets, PushVel);
}

void UCGameplayAbilityBase::PlayMontageLocally(UAnimMontage* MontageToPlay)
{
	UAnimInstance* OwnerAnimInst = GetOwnerAnimInstance();
	if (OwnerAnimInst && !OwnerAnimInst->Montage_IsPlaying(MontageToPlay))
	{
		OwnerAnimInst->Montage_Play(MontageToPlay);
	}
}

void UCGameplayAbilityBase::StopMontageAfterCurrentSection(UAnimMontage* MontageToStop)
{
	UAnimInstance* OwnerAnimInst = GetOwnerAnimInstance();
	if (OwnerAnimInst)
	{
		FName CurrentSectionName = OwnerAnimInst->Montage_GetCurrentSection(MontageToStop);
		OwnerAnimInst->Montage_SetNextSection(CurrentSectionName, NAME_None, MontageToStop);
	}
}

ACharacter* UCGameplayAbilityBase::GetOwnerAvatarCharacter()
{
	if (!OwnerAvatarCharacter)
	{
		OwnerAvatarCharacter = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	}

	return OwnerAvatarCharacter;
}

void UCGameplayAbilityBase::ApplyGameplayEffectToHitResultActor(
	const FHitResult& HitResult, TSubclassOf<UGameplayEffect> GameplayEffect, int Level)
{
	FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(GameplayEffect, Level);

	FGameplayEffectContextHandle EffectContext =
		MakeEffectContext(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo());
	EffectContext.AddHitResult(HitResult);

	EffectSpecHandle.Data->SetContext(EffectContext);

	ApplyGameplayEffectSpecToTarget(
		GetCurrentAbilitySpecHandle(),
		GetCurrentActorInfo(),
		GetCurrentActivationInfo(),
		EffectSpecHandle,
		UAbilitySystemBlueprintLibrary::AbilityTargetDataFromHitResult(HitResult)
	);
}

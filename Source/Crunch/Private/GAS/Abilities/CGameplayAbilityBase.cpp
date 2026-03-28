// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/CGameplayAbilityBase.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "CGameplayTags.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"

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

// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/GA_Passive_Dead.h"

#include "CGameplayTags.h"
#include "CrunchDebugHelper.h"
#include "Engine/OverlapResult.h"
#include "FunctionLibrary/CAbilitySystemFunctionLibrary.h"

UGA_Passive_Dead::UGA_Passive_Dead()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;

	FAbilityTriggerData TriggerData;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	TriggerData.TriggerTag = CGameplayTags::Crunch_Stats_Dead;

	AbilityTriggers.Add(TriggerData);

	ActivationBlockedTags.RemoveTag(CGameplayTags::Crunch_Stats_Stun);
}

void UGA_Passive_Dead::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	if (K2_HasAuthority())
	{
		AActor* Killer = TriggerEventData->ContextHandle.GetEffectCauser();
		if (Killer)
		{
			Debug::Print(FString::Printf(TEXT("我[%s]被[%s]击杀"), *GetAvatarActorFromActorInfo()->GetName(), *Killer->GetName()));
		}

		TArray<AActor*> RewardTargets = GetRewardTargets();
		for (const AActor* RewardTarget : RewardTargets)
		{
			Debug::Print(FString::Printf(TEXT("英雄[%s]可获得经验"), *RewardTarget->GetName()));
		}
	}
}

TArray<AActor*> UGA_Passive_Dead::GetRewardTargets() const
{
	TSet<AActor*> OutActors;

	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor || !GetWorld())
	{
		return OutActors.Array();
	}

	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);
	FCollisionShape CollisionShape;
	CollisionShape.SetSphere(RewardRange);

	TArray<FOverlapResult> OverlapResults;
	if (GetWorld()->OverlapMultiByObjectType(
		OverlapResults, AvatarActor->GetActorLocation(), FQuat::Identity, ObjectQueryParams, CollisionShape))
	{
		for (const FOverlapResult& OverlapResult : OverlapResults)
		{
			const IGenericTeamAgentInterface* OtherTeamInterface = Cast<IGenericTeamAgentInterface>(
				OverlapResult.GetActor()
			);
			if (!OtherTeamInterface ||
				OtherTeamInterface->GetTeamAttitudeTowards(*AvatarActor) != ETeamAttitude::Hostile)
			{
				continue;
			}

			if (!UCAbilitySystemFunctionLibrary::IsHero(OverlapResult.GetActor()))
			{
				continue;
			}

			OutActors.Add(OverlapResult.GetActor());
		}
	}

	return OutActors.Array();
}

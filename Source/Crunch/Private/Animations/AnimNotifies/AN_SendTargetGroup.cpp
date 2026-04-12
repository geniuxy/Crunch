// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/AnimNotifies/AN_SendTargetGroup.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemGlobals.h"
#include "GameplayCueManager.h"
#include "Kismet/KismetSystemLibrary.h"

void UAN_SendTargetGroup::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                 const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp) return;
	if (TargetSocketNames.Num() <= 1) return;
	if (!MeshComp->GetOwner() || !UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(MeshComp->GetOwner()))
	{
		return;
	}

	FGameplayEventData Data;
	TSet<AActor*> HitActors;
	AActor* OwnerActor = MeshComp->GetOwner();
	const IGenericTeamAgentInterface* OwnerTeamInterface = Cast<IGenericTeamAgentInterface>(OwnerActor);
	for (int i = 1; i < TargetSocketNames.Num(); ++i)
	{
		FVector StartLoc = MeshComp->GetSocketLocation(TargetSocketNames[i - 1]);
		FVector EndLoc = MeshComp->GetSocketLocation(TargetSocketNames[i]);

		TArray<FHitResult> HitResults;

		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

		TArray<AActor*> ActorsToIgnore;
		if (bIgnoreOwner)
		{
			ActorsToIgnore.Add(OwnerActor);
		}

		EDrawDebugTrace::Type DrawDebugTrace = bDrawDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;

		UKismetSystemLibrary::SphereTraceMultiForObjects(
			MeshComp, StartLoc, EndLoc, SphereSweepRadius, ObjectTypes, false,
			ActorsToIgnore, DrawDebugTrace, HitResults, false
		);

		for (const FHitResult& HitResult : HitResults)
		{
			if (HitActors.Contains(HitResult.GetActor())) continue;

			if (OwnerTeamInterface && OwnerTeamInterface->GetTeamAttitudeTowards(*HitResult.GetActor()) != TargetTeam)
			{
				continue;
			}

			FGameplayAbilityTargetData_SingleTargetHit* TargetHit = new FGameplayAbilityTargetData_SingleTargetHit(HitResult);
			Data.TargetData.Add(TargetHit);
			SendLocalGameplayCue(HitResult);
		}
	}

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(MeshComp->GetOwner(), EventTag, Data);
}

/*
 * 这里Client本地执行GameplayCue，无需复制，是为了减少RPC次数，提高性能
 */
void UAN_SendTargetGroup::SendLocalGameplayCue(const FHitResult& HitResult) const
{
	FGameplayCueParameters CueParams;
	CueParams.Location = HitResult.ImpactPoint;
	CueParams.Normal = HitResult.ImpactNormal;

	for (const FGameplayTag& GameplayCueTag : TriggerGameplayCueTags)
	{
		UAbilitySystemGlobals::Get().GetGameplayCueManager()->HandleGameplayCue(
			HitResult.GetActor(), GameplayCueTag, EGameplayCueEvent::Executed, CueParams
		);
	}
}

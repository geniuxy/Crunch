// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/AnimNotifies/AN_SendTargetGroup.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemGlobals.h"
#include "GameplayCueManager.h"
#include "Kismet/KismetSystemLibrary.h"

/*
 * Server (ROLE_Authority)、Local Client (ROLE_AutonomousProxy)、Other Clients (ROLE_SimulatedProxy)
 * 三种角色都会分别执行各自的 AnimNotify
 * 可作区分处理：
 * 1. 视觉效果：所有端都做（Server 可选跳过）
 * 2. 游戏逻辑：只有 Server 执行权威判定
 * 3. 本地预测反馈（Local Client 专属）
 *
 * MontageTickType 设置为 BranchingPoint，更适用于即时性要求高的，如（攻击判定、精确状态切换）
 * MontageTickType 设置为 Queued，更适用于 （音效、粒子等非关键反馈）
 * 
 * Queued 模式：由于检测窗口宽松，可能因为动画帧率波动或网络回滚，导致 同一 Notify 在同一端被多次检测到（比如 Server 一帧内触发两次）
 * BranchingPoint 模式：精确到动画评估的单一时间点，避免这种单端重复触发
 */
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

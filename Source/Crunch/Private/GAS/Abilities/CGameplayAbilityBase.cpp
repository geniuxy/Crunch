// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/CGameplayAbilityBase.h"

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
	bool bDrawDebug,
	bool bIgnoreSelf) const
{
	TArray<FHitResult> OutResults;
	TSet<AActor*> HitActors;

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

			HitActors.Add(SweepResult.GetActor());
			OutResults.Add(SweepResult);
		}
	}

	return OutResults;
}

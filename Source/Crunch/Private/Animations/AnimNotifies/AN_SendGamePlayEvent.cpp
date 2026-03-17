// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/AnimNotifies/AN_SendGamePlayEvent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayTagsManager.h"

// void UAN_SendGamePlayEvent::BranchingPointNotify(FBranchingPointNotifyPayload& BranchingPointPayload)
// {
// 	// 这里的代码设置了没有用，需要在Montage里手动设置对应的AnimNotify
// 	FAnimNotifyEventReference EventReference;
// 	FAnimNotifyEvent AnimNotifyEvent;
// 	AnimNotifyEvent.MontageTickType = EMontageNotifyTickType::BranchingPoint; // 立即执行Notify
// 	EventReference.SetNotify(&AnimNotifyEvent);
// 	
// 	Notify(BranchingPointPayload.SkelMeshComponent, BranchingPointPayload.SequenceAsset, EventReference);
// }

void UAN_SendGamePlayEvent::Notify(
	USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp->GetOwner()) return;

	UAbilitySystemComponent* OwnerASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(MeshComp->GetOwner());
	if (!OwnerASC) return;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(MeshComp->GetOwner(), EventTag, FGameplayEventData());
}

FString UAN_SendGamePlayEvent::GetNotifyName_Implementation() const
{
	if (EventTag.IsValid())
	{
		TArray<FName> TagNames;
		UGameplayTagsManager::Get().SplitGameplayTagFName(EventTag, TagNames);
		return TagNames.IsEmpty() ? "None" : TagNames.Last().ToString();
	}
	
	return "None";
}

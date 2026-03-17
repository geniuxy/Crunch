// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AN_SendGamePlayEvent.generated.h"

/**
 * 
 */
UCLASS()
class CRUNCH_API UAN_SendGamePlayEvent : public UAnimNotify
{
	GENERATED_BODY()

public:
	// virtual void BranchingPointNotify(FBranchingPointNotifyPayload& BranchingPointPayload) override;

	virtual void Notify(
		USkeletalMeshComponent* MeshComp,
		UAnimSequenceBase* Animation,
		const FAnimNotifyEventReference& EventReference
	) override;

protected:
	virtual FString GetNotifyName_Implementation() const override;

	UPROPERTY(EditAnywhere, Category="Gameplay Ability")
	FGameplayTag EventTag;
};

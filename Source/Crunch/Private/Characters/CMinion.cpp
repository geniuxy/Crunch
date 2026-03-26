// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/CMinion.h"

#include "AbilitySystemComponent.h"
#include "AIController.h"
#include "CGameplayTags.h"
#include "BehaviorTree/BlackboardComponent.h"

void ACMinion::SetGenericTeamId(const FGenericTeamId& InTeamID)
{
	Super::SetGenericTeamId(InTeamID);
	PickSkinBasedOnTeamID();
}

bool ACMinion::IsActive() const
{
	return !GetAbilitySystemComponent()->HasMatchingGameplayTag(CGameplayTags::Crunch_Stats_Dead);
}

void ACMinion::Activate()
{
	GetAbilitySystemComponent()->RemoveActiveEffectsWithGrantedTags(
		FGameplayTagContainer(CGameplayTags::Crunch_Stats_Dead)
	);
}

void ACMinion::SetGoal(AActor* InGoal)
{
	if (AAIController* AIController = GetController<AAIController>())
	{
		if (UBlackboardComponent* BlackboardComponent = AIController->GetBlackboardComponent())
		{
			BlackboardComponent->SetValueAsObject(GoalBlackboardKeyName, InGoal);
		}
	}
}

void ACMinion::OnRep_TeamID()
{
	PickSkinBasedOnTeamID();
}

void ACMinion::PickSkinBasedOnTeamID()
{
	USkeletalMesh** Skin = SkinMap.Find(GetGenericTeamId());
	if (Skin)
	{
		GetMesh()->SetSkeletalMesh(*Skin);
	}
}

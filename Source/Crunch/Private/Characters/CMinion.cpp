// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/CMinion.h"

#include "AbilitySystemComponent.h"
#include "CGameplayTags.h"

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

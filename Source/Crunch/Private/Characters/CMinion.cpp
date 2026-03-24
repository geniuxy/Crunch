// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/CMinion.h"

void ACMinion::SetGenericTeamId(const FGenericTeamId& InTeamID)
{
	Super::SetGenericTeamId(InTeamID);
	PickSkinBasedOnTeamID();
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

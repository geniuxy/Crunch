// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/CGameplayAbilityBase.h"

UAnimInstance* UCGameplayAbilityBase::GetOwnerAnimInstance() const
{
	USkeletalMeshComponent* OwnerSkeletalMeshComp = GetOwningComponentFromActorInfo();
	if (OwnerSkeletalMeshComp)
	{
		return OwnerSkeletalMeshComp->GetAnimInstance();
	}
	return nullptr;
}

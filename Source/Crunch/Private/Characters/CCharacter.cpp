// Fill out your copyright notice in the Description page of Project Settings.


#include "Crunch/Public/Characters/CCharacter.h"

#include "GAS/CAbilitySystemComponent.h"
#include "GAS/CAttributeSet.h"


ACCharacter::ACCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	AbilitySystemComponent = CreateDefaultSubobject<UCAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AttributeSet = CreateDefaultSubobject<UCAttributeSet>(TEXT("AttributeSet"));
}

UAbilitySystemComponent* ACCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}



// Fill out your copyright notice in the Description page of Project Settings.


#include "Crunch/Public/Characters/CCharacter.h"

#include "Components/WidgetComponent.h"
#include "GAS/CAbilitySystemComponent.h"
#include "GAS/CAttributeSet.h"
#include "Widgets/OverHeadStatsGauge.h"


ACCharacter::ACCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	AbilitySystemComponent = CreateDefaultSubobject<UCAbilitySystemComponent>(TEXT("Ability System Component"));
	AttributeSet = CreateDefaultSubobject<UCAttributeSet>(TEXT("AttributeSet"));

	OverHeadWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("Over Head Widget Component"));
	OverHeadWidgetComponent->SetupAttachment(GetRootComponent());
}

void ACCharacter::ServerSideInit()
{
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	AbilitySystemComponent->InitializeBaseAttributes();
}

void ACCharacter::ClientSideInit()
{
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
}

void ACCharacter::BeginPlay()
{
	Super::BeginPlay();
	ConfigureOverHeadStatsWidget();
}

UAbilitySystemComponent* ACCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ACCharacter::ConfigureOverHeadStatsWidget()
{
	if (!OverHeadWidgetComponent)
	{
		return;
	}

	UOverHeadStatsGauge* OverHeadStatsGauge = Cast<UOverHeadStatsGauge>(OverHeadWidgetComponent->GetUserWidgetObject());
	if (OverHeadStatsGauge)
	{
		OverHeadStatsGauge->ConfigureWithASC(GetAbilitySystemComponent());
	}
}

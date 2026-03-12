// Fill out your copyright notice in the Description page of Project Settings.


#include "Crunch/Public/Characters/CCharacter.h"

#include "Components/WidgetComponent.h"
#include "GAS/CAbilitySystemComponent.h"
#include "GAS/CAttributeSet.h"
#include "Kismet/GameplayStatics.h"
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

bool ACCharacter::IsLocallyControlledByPlayer()
{
	return GetController() && GetController()->IsLocalPlayerController();
}

void ACCharacter::BeginPlay()
{
	Super::BeginPlay();
	ConfigureOverHeadStatsWidget();
}

void ACCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	if (NewController && !NewController->IsPlayerController()) //专用于AICharacter在Server上的初始化
	{
		ServerSideInit();
	}
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

	if (IsLocallyControlledByPlayer())
	{
		OverHeadWidgetComponent->SetHiddenInGame(true);
		return;
	}

	UOverHeadStatsGauge* OverHeadStatsGauge = Cast<UOverHeadStatsGauge>(OverHeadWidgetComponent->GetUserWidgetObject());
	if (OverHeadStatsGauge)
	{
		OverHeadStatsGauge->ConfigureWithASC(GetAbilitySystemComponent());
		OverHeadWidgetComponent->SetHiddenInGame(false);
		GetWorldTimerManager().ClearTimer(HeadStatsGaugeVisibilityUpdateTimerHandle);
		GetWorldTimerManager().SetTimer(
			HeadStatsGaugeVisibilityUpdateTimerHandle, this, &ACCharacter::UpdateHeadGaugeVisibility,
			HeadStatsGaugeVisibilityCheckUpdateGap, true
		);
	}
}

void ACCharacter::UpdateHeadGaugeVisibility()
{
	APawn* LocalPlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	if (LocalPlayerPawn)
	{
		float DistSquared = FVector::DistSquared(GetActorLocation(), LocalPlayerPawn->GetActorLocation());
		OverHeadWidgetComponent->SetHiddenInGame(DistSquared > HeadStatsGaugeVisibilityRangeSquared);
	}
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "Crunch/Public/Player/CPlayerController.h"

#include "Blueprint/UserWidget.h"
#include "Characters/CPlayerCharacter.h"
#include "Net/UnrealNetwork.h"
#include "Widgets/GameplayWidget.h"

void ACPlayerController::OnPossess(APawn* NewPawn)
{
	Super::OnPossess(NewPawn);

	OwningPlayerCharacter = Cast<ACPlayerCharacter>(NewPawn);
	if (OwningPlayerCharacter)
	{
		OwningPlayerCharacter->ServerSideInit();
		OwningPlayerCharacter->SetGenericTeamId(TeamID);
	}
}

void ACPlayerController::AcknowledgePossession(APawn* NewPawn)
{
	Super::AcknowledgePossession(NewPawn);

	OwningPlayerCharacter = Cast<ACPlayerCharacter>(NewPawn);
	if (OwningPlayerCharacter)
	{
		OwningPlayerCharacter->ClientSideInit();
		SpawnGameplayWidget();
	}
}

void ACPlayerController::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACPlayerController, TeamID);
}

void ACPlayerController::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
	TeamID = NewTeamID;
}

FGenericTeamId ACPlayerController::GetGenericTeamId() const
{
	return TeamID;
}

void ACPlayerController::SpawnGameplayWidget()
{
	if (!IsLocalPlayerController()) // 排除在Client端，Server的副本角色的情况
	{
		return;
	}

	GameplayWidget = CreateWidget<UGameplayWidget>(this, GameplayWidgetClass);
	if (GameplayWidget)
	{
		GameplayWidget->AddToViewport();
	}
}

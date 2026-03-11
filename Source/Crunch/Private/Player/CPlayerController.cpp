// Fill out your copyright notice in the Description page of Project Settings.


#include "Crunch/Public/Player/CPlayerController.h"

#include "Characters/CPlayerCharacter.h"

void ACPlayerController::OnPossess(APawn* NewPawn)
{
	Super::OnPossess(NewPawn);

	OwningPlayerCharacter = Cast<ACPlayerCharacter>(NewPawn);
	if (OwningPlayerCharacter)
	{
		OwningPlayerCharacter->ServerSideInit();
	}
}

void ACPlayerController::AcknowledgePossession(APawn* NewPawn)
{
	Super::AcknowledgePossession(NewPawn);

	OwningPlayerCharacter = Cast<ACPlayerCharacter>(NewPawn);
	if (OwningPlayerCharacter)
	{
		OwningPlayerCharacter->ClientSideInit();
	}
}

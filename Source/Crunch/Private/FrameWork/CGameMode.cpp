// Fill out your copyright notice in the Description page of Project Settings.


#include "Crunch/Public/FrameWork/CGameMode.h"

#include "EngineUtils.h"
#include "Characters/CStormCore.h"
#include "GameFramework/PlayerStart.h"
#include "Player/CPlayerController.h"
#include "Player/CPlayerState.h"

void ACGameMode::StartPlay()
{
	Super::StartPlay();

	ACStormCore* StormCore = GetStormCore();
	if (StormCore)
	{
		StormCore->OnGoalReachedDelegate.AddUObject(this, &ThisClass::MatchFinished);
	}
}

APlayerController* ACGameMode::SpawnPlayerController(ENetRole InRemoteRole, const FString& Options)
{
	APlayerController* NewPlayerController = Super::SpawnPlayerController(InRemoteRole, Options);
	IGenericTeamAgentInterface* NewPlayerTeamInterface = Cast<IGenericTeamAgentInterface>(NewPlayerController);
	FGenericTeamId TeamID = GetTeamIDForPlayer(NewPlayerController);
	if (NewPlayerTeamInterface)
	{
		NewPlayerTeamInterface->SetGenericTeamId(TeamID);
	}

	NewPlayerController->StartSpot = FindNextStartSpotForTeam(TeamID);
	return NewPlayerController;
}

UClass* ACGameMode::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	ACPlayerState* CPlayerState = InController->GetPlayerState<ACPlayerState>();
	if (CPlayerState && CPlayerState->GetSelectedPawnClass())
	{
		return CPlayerState->GetSelectedPawnClass();
	}

	return BackUpPawn;
}

APawn* ACGameMode::SpawnDefaultPawnFor_Implementation(AController* NewPlayer, AActor* StartSpot)
{
	IGenericTeamAgentInterface* NewPlayerTeamInterface = Cast<IGenericTeamAgentInterface>(NewPlayer);
	FGenericTeamId TeamID = GetTeamIDForPlayer(NewPlayer);
	if (NewPlayerTeamInterface)
	{
		NewPlayerTeamInterface->SetGenericTeamId(TeamID);
	}

	StartSpot = FindNextStartSpotForTeam(TeamID);
	NewPlayer->StartSpot = StartSpot;

	return Super::SpawnDefaultPawnFor_Implementation(NewPlayer, StartSpot);
}

FGenericTeamId ACGameMode::GetTeamIDForPlayer(const AController* InController) const
{
	ACPlayerState* CPlayerState = InController->GetPlayerState<ACPlayerState>();
	if (CPlayerState && CPlayerState->GetSelectedPawnClass())
	{
		return CPlayerState->GetTeamIDBaseOnSlot();
	}

	static int PlayerCount = 0;
	++PlayerCount;
	return FGenericTeamId(PlayerCount % 2);
}

AActor* ACGameMode::FindNextStartSpotForTeam(const FGenericTeamId& TeamID) const
{
	const FName* StartSpotTag = TeamStartSpotTagMap.Find(TeamID);
	if (!StartSpotTag)
	{
		return nullptr;
	}

	UWorld* World = GetWorld();
	for (TActorIterator<APlayerStart> It(World); It; ++It)
	{
		if (It->PlayerStartTag == *StartSpotTag)
		{
			It->PlayerStartTag = FName("Taken");
			return *It;
		}
	}

	return nullptr;
}

ACStormCore* ACGameMode::GetStormCore() const
{
	UWorld* World = GetWorld();
	if (World)
	{
		for (TActorIterator<ACStormCore> It(World); It; ++It)
		{
			return *It;
		}
	}

	return nullptr;
}

void ACGameMode::MatchFinished(AActor* ViewTarget, int WinningTeam)
{
	UWorld* World = GetWorld();
	if (World)
	{
		for (TActorIterator<ACPlayerController> It(World); It; ++It)
		{
			It->MatchFinished(ViewTarget, WinningTeam);
		}
	}
}

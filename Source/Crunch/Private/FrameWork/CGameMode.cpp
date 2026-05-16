// Fill out your copyright notice in the Description page of Project Settings.


#include "Crunch/Public/FrameWork/CGameMode.h"

#include "CrunchDebugHelper.h"
#include "EngineUtils.h"
#include "Characters/CStormCore.h"
#include "GameFramework/PlayerStart.h"
#include "NetWork/CGameSession.h"
#include "Player/CPlayerController.h"
#include "Player/CPlayerState.h"

ACGameMode::ACGameMode()
{
	GameSessionClass = ACGameSession::StaticClass();
}

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
	if (!NewPlayer)
	{
		return nullptr;
	}

	IGenericTeamAgentInterface* NewPlayerTeamInterface = Cast<IGenericTeamAgentInterface>(NewPlayer);
	const FGenericTeamId TeamID = GetTeamIDForPlayer(NewPlayer);
	if (NewPlayerTeamInterface)
	{
		NewPlayerTeamInterface->SetGenericTeamId(TeamID);
	}

	// 尝试获取队伍专属出生点
	AActor* FinalStartSpot = FindNextStartSpotForTeam(TeamID);

	// 【关键】如果队伍出生点不足，按优先级回退：
	//  队伍出生点 -> 传入的 StartSpot -> 引擎默认查找
	if (!FinalStartSpot)
	{
		FinalStartSpot = StartSpot; // 先保留原本传入的
		if (!FinalStartSpot)
		{
			FinalStartSpot = FindPlayerStart(NewPlayer); // 再让引擎找一个可用的
		}
	}

	// 【防御】如果所有方式都找不到，记录日志并直接返回 nullptr
	//  避免把 nullptr 传给 Super 导致底层崩溃
	if (!FinalStartSpot)
	{
		Debug::Print(FString::Printf(
				TEXT("SpawnDefaultPawnFor: No valid StartSpot found for player %s (TeamID: %d). Aborting pawn spawn."),
				*GetNameSafe(NewPlayer), TeamID.GetId())
		);
		return nullptr;
	}

	// 确认有效后再赋值和调用父类
	NewPlayer->StartSpot = FinalStartSpot;
	return Super::SpawnDefaultPawnFor_Implementation(NewPlayer, FinalStartSpot);
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

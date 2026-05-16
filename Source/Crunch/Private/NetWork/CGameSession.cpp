// Fill out your copyright notice in the Description page of Project Settings.


#include "NetWork/CGameSession.h"

#include "FrameWork/CGameInstance.h"

bool ACGameSession::ProcessAutoLogin()
{
	return true;
}

void ACGameSession::RegisterPlayer(APlayerController* NewPlayer, const FUniqueNetIdRepl& UniqueId, bool bWasFromInvite)
{
	Super::RegisterPlayer(NewPlayer, UniqueId, bWasFromInvite);
	if (GetWorld()->IsEditorWorld()) return;
	if (UCGameInstance* GameInst = GetGameInstance<UCGameInstance>())
	{
		GameInst->PlayerJoined(UniqueId);
	}
}

void ACGameSession::UnregisterPlayer(FName InSessionName, const FUniqueNetIdRepl& UniqueId)
{
	Super::UnregisterPlayer(InSessionName, UniqueId);
	if (GetWorld()->IsEditorWorld()) return;
	if (UCGameInstance* GameInst = GetGameInstance<UCGameInstance>())
	{
		GameInst->PlayerLeft(UniqueId);
	}
}

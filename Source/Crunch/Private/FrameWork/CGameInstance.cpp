// Fill out your copyright notice in the Description page of Project Settings.


#include "FrameWork/CGameInstance.h"

#include "CrunchDebugHelper.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "FunctionLibrary/NetFunctionLibrary.h"

void UCGameInstance::StartMatch()
{
	if (GetWorld()->GetNetMode() == NM_DedicatedServer || GetWorld()->GetNetMode() == NM_ListenServer)
	{
		LoadLevelAndListen(GameLevel);
	}
}

void UCGameInstance::Init()
{
	Super::Init();
	if (GetWorld()->IsEditorWorld())return;
	if (UNetFunctionLibrary::IsSessionServer(this))
	{
		CreateSession();
	}
}

void UCGameInstance::CreateSession()
{
	IOnlineSessionPtr SessionPtr = UNetFunctionLibrary::GetSessionPtr();
	if (SessionPtr)
	{
		ServerSessionName = UNetFunctionLibrary::GetSessionNameStr();
		FString SessionSearchId = UNetFunctionLibrary::GetSessionSearchIdStr();
		SessionServerPort = UNetFunctionLibrary::GetSessionPort();
		Debug::Print(FString::Printf(
				TEXT("#### Create Session With Name: %s, ID: %s, Port: %d"),
				*(ServerSessionName), *(SessionSearchId), SessionServerPort)
		);

		FOnlineSessionSettings OnlineSessionSettings = UNetFunctionLibrary::GenerateOnlineSessionSettings(
			FName(ServerSessionName), SessionSearchId, SessionServerPort
		);
		if (!SessionPtr->CreateSession(0, FName(ServerSessionName), OnlineSessionSettings))
		{
			Debug::Print(TEXT("Session Creating Failed Right away!!!!"));
		}
	}
}

void UCGameInstance::LoadLevelAndListen(TSoftObjectPtr<UWorld> Level)
{
	const FName LevelURL = FName(*FPackageName::ObjectPathToPackageName(Level.ToString()));

	if (LevelURL != "")
	{
		GetWorld()->ServerTravel(LevelURL.ToString() + "?listen");
	}
}

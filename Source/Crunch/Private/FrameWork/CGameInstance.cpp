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

void UCGameInstance::PlayerJoined(const FUniqueNetIdRepl& UniqueId)
{
	if (WaitPlayerJoinTimeoutHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(WaitPlayerJoinTimeoutHandle);
	}
	
	PlayerRecord.Add(UniqueId);
}

void UCGameInstance::PlayerLeft(const FUniqueNetIdRepl& UniqueId)
{
	PlayerRecord.Remove(UniqueId);

	if (PlayerRecord.Num() == 0)
	{
		Debug::Print(TEXT("All player left the session, terminating"));
		TerminateSessionServer();
	}
}

void UCGameInstance::CreateSession()
{
	IOnlineSessionPtr SessionPtr = UNetFunctionLibrary::GetSessionPtr();
	if (SessionPtr)
	{
		ServerSessionName = UNetFunctionLibrary::GetSessionNameStr();
		FString SessionSearchID = UNetFunctionLibrary::GetSessionSearchIDStr();
		SessionServerPort = UNetFunctionLibrary::GetSessionPort();
		Debug::Print(FString::Printf(
				TEXT("#### Create Session With Name: %s, ID: %s, Port: %d"),
				*(ServerSessionName), *(SessionSearchID), SessionServerPort)
		);

		FOnlineSessionSettings OnlineSessionSettings = UNetFunctionLibrary::GenerateOnlineSessionSettings(
			FName(ServerSessionName), SessionSearchID, SessionServerPort
		);
		SessionPtr->OnCreateSessionCompleteDelegates.RemoveAll(this);
		SessionPtr->OnCreateSessionCompleteDelegates.AddUObject(this, &ThisClass::OnSessionCreated);
		if (!SessionPtr->CreateSession(0, FName(ServerSessionName), OnlineSessionSettings))
		{
			Debug::Print(TEXT("Session Creating Failed Right away!!!!"));
			SessionPtr->OnCreateSessionCompleteDelegates.RemoveAll(this);
			TerminateSessionServer();
		}
	}
	else
	{
		Debug::Print(TEXT("Can't find SessionPtr, Terminating"));
		TerminateSessionServer();
	}
}

void UCGameInstance::OnSessionCreated(FName SessionName, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		Debug::Print(TEXT("------------------ Session Created Successful!"));
		GetWorld()->GetTimerManager().SetTimer(
			WaitPlayerJoinTimeoutHandle, this, &ThisClass::WaitPlayerJoinTimeOutReached, WaitPlayerJoinTimeOutDuration
		);
		LoadLevelAndListen(LobbyLevel);
	}
	else
	{
		Debug::Print(TEXT("------------------ Session Created Failed"));
		TerminateSessionServer();
	}

	if (IOnlineSessionPtr SessionPtr = UNetFunctionLibrary::GetSessionPtr())
	{
		SessionPtr->OnCreateSessionCompleteDelegates.RemoveAll(this);
	}
}

void UCGameInstance::EndSessionCompleted(FName SessionName, bool bWasSuccessful)
{
	FGenericPlatformMisc::RequestExit(false);
}

void UCGameInstance::TerminateSessionServer()
{
	if (IOnlineSessionPtr SessionPtr = UNetFunctionLibrary::GetSessionPtr())
	{
		SessionPtr->OnEndSessionCompleteDelegates.RemoveAll(this);
		SessionPtr->OnEndSessionCompleteDelegates.AddUObject(this, &ThisClass::EndSessionCompleted);
		if (!SessionPtr->EndSession(FName(ServerSessionName)))
		{
			FGenericPlatformMisc::RequestExit(false);
		}
	}
	else
	{
		FGenericPlatformMisc::RequestExit(false);
	}
}

void UCGameInstance::WaitPlayerJoinTimeOutReached()
{
	Debug::Print(FString::Printf(
			TEXT("Session Sever shut down after %.1f seconds without player joining"), WaitPlayerJoinTimeOutDuration)
	);
	TerminateSessionServer();
}

void UCGameInstance::LoadLevelAndListen(TSoftObjectPtr<UWorld> Level)
{
	const FName LevelURL = FName(*FPackageName::ObjectPathToPackageName(Level.ToString()));

	if (LevelURL != "")
	{
		FString TravelStr = "";
		if (SessionServerPort == 0)
		{
			TravelStr = FString::Printf(TEXT("%s?listen"), *LevelURL.ToString());
		}
		else
		{
			TravelStr = FString::Printf(TEXT("%s?listen?port=%d"), *LevelURL.ToString(), SessionServerPort);
		}
		Debug::Print(FString::Printf(TEXT("Server traveling to: %s"), *TravelStr));
		GetWorld()->ServerTravel(TravelStr);
	}
}

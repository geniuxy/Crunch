// Fill out your copyright notice in the Description page of Project Settings.


#include "FrameWork/CGameInstance.h"

#include "CrunchDebugHelper.h"
#include "HttpModule.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "FunctionLibrary/NetFunctionLibrary.h"
#include "Interfaces/IHttpRequest.h"

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

bool UCGameInstance::IsLoggedIn() const
{
	if (IOnlineIdentityPtr IdentityPtr = UNetFunctionLibrary::GetIdentityPtr())
	{
		return IdentityPtr->GetLoginStatus(0) == ELoginStatus::LoggedIn;
	}
	return false;
}

bool UCGameInstance::IsLoggingIn() const
{
	return LoggingInDelegateHandle.IsValid();
}

void UCGameInstance::ClientAccountPortalLogin()
{
	ClientLogin("AccountPortal", "", "");
}

void UCGameInstance::ClientLogin(const FString& Type, const FString& ID, const FString& Token)
{
	if (IOnlineIdentityPtr IdentityPtr = UNetFunctionLibrary::GetIdentityPtr())
	{
		if (LoggingInDelegateHandle.IsValid())
		{
			IdentityPtr->OnLoginCompleteDelegates->Remove(LoggingInDelegateHandle);
			LoggingInDelegateHandle.Reset();
		}

		LoggingInDelegateHandle = IdentityPtr->OnLoginCompleteDelegates->AddUObject(this, &ThisClass::LoginCompleted);
		if (!IdentityPtr->Login(0, FOnlineAccountCredentials(Type, ID, Token)))
		{
			Debug::Print(TEXT("登入失败"));
			if (LoggingInDelegateHandle.IsValid())
			{
				IdentityPtr->OnLoginCompleteDelegates->Remove(LoggingInDelegateHandle);
				LoggingInDelegateHandle.Reset();
			}
			OnLoginCompleted.Broadcast(false, "", "登入失败");
		}
	}
}

void UCGameInstance::LoginCompleted(
	int NumOfLocalPlayer, bool bWasSuccessful, const FUniqueNetId& UserID, const FString& Error)
{
	if (IOnlineIdentityPtr IdentityPtr = UNetFunctionLibrary::GetIdentityPtr())
	{
		if (LoggingInDelegateHandle.IsValid())
		{
			IdentityPtr->OnLoginCompleteDelegates->Remove(LoggingInDelegateHandle);
			LoggingInDelegateHandle.Reset();
		}

		FString PlayerNickName = "";
		if (bWasSuccessful)
		{
			PlayerNickName = IdentityPtr->GetPlayerNickname(UserID);
			Debug::Print(FString::Printf(TEXT("登录成功，用户名为: %s"), *(PlayerNickName)));
		}
		else
		{
			Debug::Print(FString::Printf(TEXT("登录失败，原因为: %s"), *(Error)));
		}

		OnLoginCompleted.Broadcast(bWasSuccessful, PlayerNickName, Error);
	}
	else
	{
		OnLoginCompleted.Broadcast(false, "", TEXT("Can't find the Identity Pointer"));
	}
}

void UCGameInstance::RequestCreateAndJoinSession(const FName& NewSessionName)
{
	Debug::Print(TEXT("请求创建并加入房间"), NewSessionName.ToString());
	FHttpRequestRef Request = FHttpModule::Get().CreateRequest();
	FGuid SessionSearchID = FGuid::NewGuid();

	FString CoordinatorURL = UNetFunctionLibrary::GetCoordinatorURL();
	FString URL = FString::Printf(TEXT("%s/Sessions"), *CoordinatorURL);
	Debug::Print(TEXT("发送创建房间请求至URL"), URL);

	Request->SetURL(URL);
	Request->SetVerb("Post");
	Request->SetHeader(TEXT("Content-type"), TEXT("application/json"));

	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	JsonObject->SetStringField(UNetFunctionLibrary::GetSessionNameKey().ToString(), NewSessionName.ToString());
	JsonObject->SetStringField(UNetFunctionLibrary::GetSessionSearchIDKey().ToString(), SessionSearchID.ToString());

	FString RequestBody;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

	Request->SetContentAsString(RequestBody);
	Request->OnProcessRequestComplete().BindUObject(this, &ThisClass::SessionCreationRequestCompleted, SessionSearchID);

	if (!Request->ProcessRequest())
	{
		Debug::Print(TEXT("请求创建房间失败！"));
	}
}

void UCGameInstance::CancelSessionCreation()
{
	Debug::Print(TEXT("取消创建房间"));
}

void UCGameInstance::SessionCreationRequestCompleted(
	FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully, FGuid SessionSearchID)
{
	if (!bConnectedSuccessfully)
	{
		Debug::Print(TEXT("连接至协调器地址失败！"));
		return;
	}

	Debug::Print(TEXT("连接至协调器地址成功！"));
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

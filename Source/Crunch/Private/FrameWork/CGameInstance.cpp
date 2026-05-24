// Fill out your copyright notice in the Description page of Project Settings.


#include "FrameWork/CGameInstance.h"

#include "CrunchDebugHelper.h"
#include "HttpModule.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "FunctionLibrary/NetFunctionLibrary.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"

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
	StopAllSessionFindings();

	if (IOnlineSessionPtr SessionPtr = UNetFunctionLibrary::GetSessionPtr())
	{
		SessionPtr->OnFindSessionsCompleteDelegates.RemoveAll(this);
		SessionPtr->OnJoinSessionCompleteDelegates.RemoveAll(this);
	}

	StartGlobalSessionSearch();
}

void UCGameInstance::StartGlobalSessionSearch()
{
	Debug::Print(TEXT("开始全局会话搜寻"));
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
	int32 ResponseCode = Response->GetResponseCode();
	if (ResponseCode != 200)
	{
		Debug::Print(TEXT("房间会话创建失败, 错误代码为"), ResponseCode);
		return;
	}

	FString ResponseStr = Response->GetContentAsString();

	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseStr);
	int32 Port = 0;

	if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
	{
		Port = JsonObject->GetIntegerField(*UNetFunctionLibrary::GetPortKey().ToString());
	}

	Debug::Print(TEXT("连接至协调器地址成功，并且新房间会话创建于端口"), Port);
	StartFindingCreatedSession(SessionSearchID);
}

void UCGameInstance::StartFindingCreatedSession(const FGuid& SessionSearchId)
{
	if (!SessionSearchId.IsValid())
	{
		Debug::Print(TEXT("SessionSearchId（会话搜寻ID）无效，无法开始寻找会话!"));
		return;
	}
	StopAllSessionFindings();
	Debug::Print(TEXT("开始寻找创建的房间会话，SessionSearchId（会话搜寻ID）为"), SessionSearchId.ToString());

	GetWorld()->GetTimerManager().SetTimer(
		FindCreatedSessionTimerHandle,
		FTimerDelegate::CreateUObject(this, &ThisClass::FindCreatedSession, SessionSearchId),
		FindCreatedSessionSearchInterval,
		true,
		0.f
	);

	GetWorld()->GetTimerManager().SetTimer(
		FindCreatedSessionTimeoutHandle,
		this,
		&ThisClass::FindCreatedSessionTimeout,
		FindCreatedSessionTimeoutDuration
	);
}

void UCGameInstance::StopAllSessionFindings()
{
	Debug::Print(TEXT("暂停所有的搜寻会话行为."));
	StopFindingCreatedSession();
	StopGlobalSessionSearch();
}

void UCGameInstance::StopFindingCreatedSession()
{
	Debug::Print(TEXT("暂停寻找创建的会话.."));
	GetWorld()->GetTimerManager().ClearTimer(FindCreatedSessionTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(FindCreatedSessionTimeoutHandle);

	if (IOnlineSessionPtr SessionPtr = UNetFunctionLibrary::GetSessionPtr())
	{
		SessionPtr->OnFindSessionsCompleteDelegates.RemoveAll(this);
		SessionPtr->OnJoinSessionCompleteDelegates.RemoveAll(this);
	}
}

void UCGameInstance::StopGlobalSessionSearch()
{
	Debug::Print(TEXT("暂停搜寻全局会话.."));
}

void UCGameInstance::FindCreatedSession(FGuid SessionSearchID)
{
	Debug::Print(TEXT("尝试搜寻所创建的会话~~~~~~"));
	IOnlineSessionPtr SessionPtr = UNetFunctionLibrary::GetSessionPtr();
	if (!SessionPtr)
	{
		Debug::Print(TEXT("找不到SessionPtr, 取消搜索会话"));
		return;
	}

	SessionSearch = MakeShareable(new FOnlineSessionSearch);
	if (!SessionSearch)
	{
		Debug::Print(TEXT("创建SessionSearch失败, 取消搜索会话"));
		return;
	}

	SessionSearch->bIsLanQuery = false;
	SessionSearch->MaxSearchResults = 1;
	SessionSearch->QuerySettings.Set(
		UNetFunctionLibrary::GetSessionSearchIDKey(), SessionSearchID.ToString(), EOnlineComparisonOp::Equals
	);

	SessionPtr->OnFindSessionsCompleteDelegates.RemoveAll(this);
	SessionPtr->OnFindSessionsCompleteDelegates.AddUObject(this, &ThisClass::FindCreatedSessionCompleted);
	if (!SessionPtr->FindSessions(0, SessionSearch.ToSharedRef()))
	{
		Debug::Print(TEXT("寻找创建会话失败！......"));
		SessionPtr->OnFindSessionsCompleteDelegates.RemoveAll(this);
	}
}

void UCGameInstance::FindCreatedSessionTimeout()
{
	Debug::Print(TEXT("搜寻所创建的会话超时......"));
	StopFindingCreatedSession();
}

void UCGameInstance::FindCreatedSessionCompleted(bool bWasSuccessful)
{
	if (!bWasSuccessful || SessionSearch->SearchResults.Num() == 0)
	{
		return;
	}

	StopFindingCreatedSession();
	JoinSessionWithSearchResult(SessionSearch->SearchResults[0]);
}

void UCGameInstance::JoinSessionWithSearchResult(const FOnlineSessionSearchResult& SearchResult)
{
	Debug::Print(TEXT("搜寻到会话结果，加入会话！"));
	IOnlineSessionPtr SessionPtr = UNetFunctionLibrary::GetSessionPtr();
	if (!SessionPtr)
	{
		Debug::Print(TEXT("找不到SessionPtr, 取消加入会话"));
		return;
	}

	FString SessionName = "";
	SearchResult.Session.SessionSettings.Get<FString>(UNetFunctionLibrary::GetSessionNameKey(), SessionName);

	const FOnlineSessionSetting* PortSetting =
		SearchResult.Session.SessionSettings.Settings.Find(UNetFunctionLibrary::GetPortKey());
	int64 Port = 7777;
	PortSetting->Data.GetValue(Port);

	Debug::Print(FString::Printf(TEXT("尝试加入会话：%s, 端口：%lld"), *SessionName, Port));
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

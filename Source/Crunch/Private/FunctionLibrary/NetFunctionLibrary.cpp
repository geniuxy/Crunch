// Fill out your copyright notice in the Description page of Project Settings.


#include "FunctionLibrary/NetFunctionLibrary.h"

#include "OnlineSessionSettings.h"

FOnlineSessionSettings UNetFunctionLibrary::GenerateOnlineSessionSettings(
	const FName& SessionName, const FString& SessionSearchID, int Port)
{
	FOnlineSessionSettings OnlineSessionSettings;
	OnlineSessionSettings.bIsLANMatch = false;
	OnlineSessionSettings.NumPublicConnections = GetPlayerCountPerTeam() * 2;
	OnlineSessionSettings.bShouldAdvertise = true;
	OnlineSessionSettings.bUsesPresence = false;
	OnlineSessionSettings.bAllowJoinViaPresence = false;
	OnlineSessionSettings.bAllowJoinViaPresenceFriendsOnly = false;
	OnlineSessionSettings.bAllowInvites = true;
	OnlineSessionSettings.bAllowJoinInProgress = false;
	OnlineSessionSettings.bUseLobbiesIfAvailable = false;
	OnlineSessionSettings.bUseLobbiesVoiceChatIfAvailable = false;
	OnlineSessionSettings.bUsesStats = true;

	OnlineSessionSettings.Set(
		GetSessionNameKey(), SessionName.ToString(), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing
	);
	OnlineSessionSettings.Set(
		GetSessionSearchIDKey(), SessionSearchID, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing
	);
	OnlineSessionSettings.Set(
		GetPortKey(), Port, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing
	);

	return OnlineSessionSettings;
}

IOnlineSessionPtr UNetFunctionLibrary::GetSessionPtr()
{
	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	if (OnlineSubsystem)
	{
		return OnlineSubsystem->GetSessionInterface();
	}
	return nullptr;
}

IOnlineIdentityPtr UNetFunctionLibrary::GetIdentityPtr()
{
	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	if (OnlineSubsystem)
	{
		return OnlineSubsystem->GetIdentityInterface();
	}
	return nullptr;
}

uint8 UNetFunctionLibrary::GetPlayerCountPerTeam()
{
	return 5;
}

bool UNetFunctionLibrary::IsSessionServer(const UObject* WorldContextObject)
{
	return WorldContextObject->GetWorld()->GetNetMode() == NM_DedicatedServer;
}

FString UNetFunctionLibrary::GetSessionNameStr()
{
	return GetCommandLineArgAsString(GetSessionNameKey());
}

FName UNetFunctionLibrary::GetSessionNameKey()
{
	return FName("SESSION_NAME");
}

FString UNetFunctionLibrary::GetSessionSearchIDStr()
{
	return GetCommandLineArgAsString(GetSessionSearchIDKey());
}

FName UNetFunctionLibrary::GetSessionSearchIDKey()
{
	return FName("SESSION_SEARCH_ID");
}

int UNetFunctionLibrary::GetSessionPort()
{
	return GetCommandLineArgAsInt(GetPortKey());
}

FName UNetFunctionLibrary::GetPortKey()
{
	return FName("PORT");
}

FString UNetFunctionLibrary::GetCommandLineArgAsString(const FName& ParamName)
{
	FString OutVal = "";
	FString CommandLineArg = FString::Printf(TEXT("%s="), *(ParamName.ToString()));
	FParse::Value(FCommandLine::Get(), *CommandLineArg, OutVal);
	return OutVal;
}

int UNetFunctionLibrary::GetCommandLineArgAsInt(const FName& ParamName)
{
	int OutVal = 0;
	FString CommandLineArg = FString::Printf(TEXT("%s="), *(ParamName.ToString()));
	FParse::Value(FCommandLine::Get(), *CommandLineArg, OutVal);
	return OutVal;
}

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "OnlineSubsystem.h"
#include "NetFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class CRUNCH_API UNetFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static FOnlineSessionSettings GenerateOnlineSessionSettings(const FName& SessionName, const FString& SessionSearchID, int Port);

	static IOnlineSessionPtr GetSessionPtr();
	static IOnlineIdentityPtr GetIdentityPtr();
	
	static uint8 GetPlayerCountPerTeam();

	static bool IsSessionServer(const UObject* WorldContextObject);

	static FString GetSessionNameStr();
	static FName GetSessionNameKey();

	static FString GetSessionSearchIDStr();
	static FName GetSessionSearchIDKey();

	static int GetSessionPort();
	static FName GetPortKey();

	static FName GetCoordinatorURLKey();
	static FString GetCoordinatorURL();
	static FString GetDefaultCoordinatorURL();

	static FString GetTestingURL();
	static FName GetTestingURLKey();
	static void ReplacePort(FString& OutURLStr, int NewPort);

	static FString GetCommandLineArgAsString(const FName& ParamName);
	static int GetCommandLineArgAsInt(const FName& ParamName);
};

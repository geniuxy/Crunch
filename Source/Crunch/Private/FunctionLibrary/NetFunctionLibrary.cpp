// Fill out your copyright notice in the Description page of Project Settings.


#include "FunctionLibrary/NetFunctionLibrary.h"

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

FString UNetFunctionLibrary::GetSessionSearchIdStr()
{
	return GetCommandLineArgAsString(GetSessionSearchIdKey());
}

FName UNetFunctionLibrary::GetSessionSearchIdKey()
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

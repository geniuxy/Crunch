// Fill out your copyright notice in the Description page of Project Settings.


#include "FrameWork/GameInstanceSubsystem/CDataSubsystem.h"

#include "FrameWork/DeveloperSetting/CDataDeveloperSetting.h"

UCDataSubsystem* UCDataSubsystem::Get(const UObject* WorldContextObject)
{
	if (GEngine)
	{
		UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::Assert);

		return UGameInstance::GetSubsystem<UCDataSubsystem>(World->GetGameInstance());
	}

	return nullptr;
}

void UCDataSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	InitializeAbilityData();
}

void UCDataSubsystem::InitializeAbilityData()
{
	const UCDataDeveloperSetting* DataDeveloperSettings = GetDefault<UCDataDeveloperSetting>();
	CachedAbilityDataTable = DataDeveloperSettings->GetAbilityDataTable();
}

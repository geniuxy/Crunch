// Fill out your copyright notice in the Description page of Project Settings.


#include "FrameWork/EngineSubsystem/CDataSubsystem.h"

#include "FrameWork/DeveloperSetting/CDataDeveloperSetting.h"

UCDataSubsystem* UCDataSubsystem::Get()
{
	return GEngine ? GEngine->GetEngineSubsystem<UCDataSubsystem>() : nullptr;
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

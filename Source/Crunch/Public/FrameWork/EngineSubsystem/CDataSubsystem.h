// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CDataSubsystem.generated.h"

/**
 * UEngineSubsystem相比UGameInstanceSubsystem的好处：
 * 1. UEngineSubsystem 没有 World 依赖，CDO 上也能正常访问。
 * 2. 目前我只存放静态的数据
 */
UCLASS()
class CRUNCH_API UCDataSubsystem : public UEngineSubsystem
{
	GENERATED_BODY()

public:
	static UCDataSubsystem* Get();

	//~Begin USubsystem Function
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	//~End USubsystem Function
	
	/**********************************************************************/
	/*                         Gameplay Ability                           */
	/**********************************************************************/
	
private:
	UPROPERTY(Transient)
	UDataTable* CachedAbilityDataTable;

	void InitializeAbilityData();

public:
	FORCEINLINE UDataTable* GetAbilityDataTable() const { return CachedAbilityDataTable; }
};

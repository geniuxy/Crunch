// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "CDataSubsystem.generated.h"

/**
 * UGameInstanceSubsystem 本质上不适用于网络同步场景，但是Client和Server上都存在，适合存放静态的数据
 * 需要网络同步时：AGameState全局游戏状态 / APlayerState玩家相关状态
 */
UCLASS()
class CRUNCH_API UCDataSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	static UCDataSubsystem* Get(const UObject* WorldContextObject);

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

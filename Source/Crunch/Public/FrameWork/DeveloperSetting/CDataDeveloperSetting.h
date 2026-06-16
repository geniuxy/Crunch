// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "CDataDeveloperSetting.generated.h"

/**
 * 
 */
UCLASS(Config = Game, DefaultConfig, meta = (DisplayName = "Data Settings"))
class CRUNCH_API UCDataDeveloperSetting : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, EditAnywhere, Category = "GAS")
	TSoftObjectPtr<UDataTable> AbilityDataTable;

	// 获取加载后的 DataTable
	UDataTable* GetAbilityDataTable() const;

private:
	UPROPERTY(Transient)
	mutable TWeakObjectPtr<UDataTable> CachedAbilityDataTable;
};

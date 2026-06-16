// Fill out your copyright notice in the Description page of Project Settings.


#include "FrameWork/DeveloperSetting/CDataDeveloperSetting.h"

UDataTable* UCDataDeveloperSetting::GetAbilityDataTable() const
{
	if (!CachedAbilityDataTable.IsValid() && !AbilityDataTable.IsNull())
	{
		CachedAbilityDataTable = AbilityDataTable.LoadSynchronous();
	}
	return CachedAbilityDataTable.Get();
}

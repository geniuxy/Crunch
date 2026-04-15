// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CTypes/CStruct.h"
#include "Engine/AssetManager.h"
#include "Inventory/Data/PA_ShopItem.h"
#include "CAssetManager.generated.h"

class UPA_ShopItem;
/**
 * 
 */
UCLASS()
class CRUNCH_API UCAssetManager : public UAssetManager
{
	GENERATED_BODY()

public:
	static UCAssetManager& Get();

	void LoadShopItems(const FStreamableDelegate& LoadFinishedCallback); // 异步加载ShopItems
	bool GetLoadedShopItems(TArray<UPA_ShopItem*>& OutItems) const; // 获取ShopItem的合集

private:
	void ShopItemLoadFinished(FStreamableDelegate Callback);
	void BuildItemMaps();
	void AddToCombinationMap(const UPA_ShopItem* Ingredient, const UPA_ShopItem* CombinationItem);
	
	UPROPERTY()
	TMap<const UPA_ShopItem*, FItemCollection> CombinationMap; // Item的后续合成装备

	UPROPERTY()
	TMap<const UPA_ShopItem*, FItemCollection> IngredientMap; // Item的配件
};

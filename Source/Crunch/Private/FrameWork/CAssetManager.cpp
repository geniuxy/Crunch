// Fill out your copyright notice in the Description page of Project Settings.


#include "FrameWork/CAssetManager.h"

#include "CrunchDebugHelper.h"
#include "CTypes/CStruct.h"

UCAssetManager& UCAssetManager::Get()
{
	UCAssetManager* Singleton = Cast<UCAssetManager>(GEngine->AssetManager.Get());
	if (Singleton)
	{
		return *Singleton;
	}

	Debug::Print(TEXT("AssetManager的类型需要是CAssetManager"));
	return *NewObject<UCAssetManager>();
}

void UCAssetManager::LoadShopItems(const FStreamableDelegate& LoadFinishedCallback)
{
	LoadPrimaryAssetsWithType(
		UPA_ShopItem::GetShopItemAssetType(),
		TArray<FName>(),
		FStreamableDelegate::CreateUObject(this, &ThisClass::ShopItemLoadFinished, LoadFinishedCallback)
	);
}

bool UCAssetManager::GetLoadedShopItems(TArray<UPA_ShopItem*>& OutItems) const
{
	TArray<UObject*> LoadedObjects;
	bool bLoaded = GetPrimaryAssetObjectList(UPA_ShopItem::GetShopItemAssetType(), LoadedObjects);
	if (bLoaded)
	{
		for (UObject* LoadedObject : LoadedObjects)
		{
			OutItems.Add(Cast<UPA_ShopItem>(LoadedObject));
		}
	}
	return bLoaded;
}

const FItemCollection* UCAssetManager::GetCombinationForItem(const UPA_ShopItem* Item) const
{
	return CombinationMap.Find(Item);
}

const FItemCollection* UCAssetManager::GetIngredientForItem(const UPA_ShopItem* Item) const
{
	return IngredientMap.Find(Item);
}

void UCAssetManager::ShopItemLoadFinished(FStreamableDelegate Callback)
{
	Callback.ExecuteIfBound();
	BuildItemMaps();
}

void UCAssetManager::BuildItemMaps()
{
	TArray<UPA_ShopItem*> LoadedItems;
	if (GetLoadedShopItems(LoadedItems))
	{
		for (const UPA_ShopItem* Item : LoadedItems)
		{
			if (Item->GetIngredients().Num() == 0) continue;

			TArray<const UPA_ShopItem*> IngredientItems;
			for (const TSoftObjectPtr<UPA_ShopItem>& Ingredient : Item->GetIngredients())
			{
				UPA_ShopItem* IngredientItem = Ingredient.LoadSynchronous();
				IngredientItems.Add(IngredientItem);
				AddToCombinationMap(IngredientItem, Item);
			}

			IngredientMap.Add(Item, FItemCollection(IngredientItems));
		}
	}
}

void UCAssetManager::AddToCombinationMap(const UPA_ShopItem* Ingredient, const UPA_ShopItem* CombinationItem)
{
	if (FItemCollection* Combinations = CombinationMap.Find(Ingredient))
	{
		if (!Combinations->Contains(CombinationItem))
		{
			Combinations->AddItem(CombinationItem);
		}
	}
	else
	{
		CombinationMap.Add(Ingredient, FItemCollection(TArray{CombinationItem}));
	}
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/Data/PA_ShopItem.h"

FPrimaryAssetId UPA_ShopItem::GetPrimaryAssetId() const
{
	return FPrimaryAssetId(GetShopItemAssetType(), GetFName());
}

FPrimaryAssetType UPA_ShopItem::GetShopItemAssetType()
{
	return FPrimaryAssetType("ShopItem");
}

UTexture2D* UPA_ShopItem::GetIcon() const
{
	return Icon.LoadSynchronous();
}

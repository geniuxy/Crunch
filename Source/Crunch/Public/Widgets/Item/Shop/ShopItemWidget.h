// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Widgets/Item/ItemWidget.h"
#include "ShopItemWidget.generated.h"

class UShopItemWidget;
class UPA_ShopItem;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnShopItemPurchaseIssued, const UPA_ShopItem*);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnShopItemSelected, const UShopItemWidget*);
/**
 * 
 */
UCLASS()
class CRUNCH_API UShopItemWidget : public UItemWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

public:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

	FOnShopItemPurchaseIssued OnShopItemPurchaseIssued;
	FOnShopItemSelected OnShopItemSelected;
private:
	UPROPERTY()
	const UPA_ShopItem* ShopItem;

	virtual void RightButtonClicked() override;
	virtual void LeftButtonClicked() override;

public:
	FORCEINLINE const UPA_ShopItem* GetShopItem() const { return ShopItem; }
};

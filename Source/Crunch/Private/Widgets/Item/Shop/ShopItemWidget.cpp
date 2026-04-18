// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Item/Shop/ShopItemWidget.h"

#include "Inventory/Data/PA_ShopItem.h"

void UShopItemWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	ShopItem = Cast<UPA_ShopItem>(ListItemObject);
	if (!ShopItem) return;

	SetIcon(ShopItem->GetIcon());
	SetToolTipWidget(ShopItem);
}

void UShopItemWidget::RightButtonClicked()
{
	OnShopItemPurchaseIssued.Broadcast(GetShopItem());
}

void UShopItemWidget::LeftButtonClicked()
{
	OnShopItemSelected.Broadcast(this);
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Item/Shop/ShopWidget.h"

#include "Components/TileView.h"
#include "FrameWork/CAssetManager.h"
#include "Inventory/InventoryComponent.h"
#include "Widgets/Item/ItemTreeWidget.h"
#include "Widgets/Item/Shop/ShopItemWidget.h"

void UShopWidget::NativeConstruct()
{
	Super::NativeConstruct();
	SetIsFocusable(true);
	LoadShopItems();
	ShopItemList->OnEntryWidgetGenerated().AddUObject(this, &ThisClass::ShopItemWidgetGenerated);
	if (APawn* OwnerPawn = GetOwningPlayerPawn())
	{
		OwnerInventoryComponent = OwnerPawn->GetComponentByClass<UInventoryComponent>();
	}
}

void UShopWidget::LoadShopItems()
{
	UCAssetManager::Get().LoadShopItems(FStreamableDelegate::CreateUObject(this, &ThisClass::ShopItemLoadFinished));
}

void UShopWidget::ShopItemLoadFinished()
{
	TArray<UPA_ShopItem*> ShopItems;
	UCAssetManager::Get().GetLoadedShopItems(ShopItems);
	for (UPA_ShopItem* ShopItem : ShopItems)
	{
		ShopItemList->AddItem(ShopItem);
	}
}

void UShopWidget::ShopItemWidgetGenerated(UUserWidget& NewWidget)
{
	UShopItemWidget* ItemWidget = Cast<UShopItemWidget>(&NewWidget);
	if (ItemWidget)
	{
		if (OwnerInventoryComponent)
		{
			ItemWidget->OnShopItemPurchaseIssued.AddUObject(OwnerInventoryComponent, &UInventoryComponent::TryPurchase);
		}
		ItemWidget->OnShopItemSelected.AddUObject(this, &ThisClass::ShowItemCombination);
		ItemsMap.Add(ItemWidget->GetShopItem(), ItemWidget);
	}
}

void UShopWidget::ShowItemCombination(const UShopItemWidget* ItemWidget)
{
	if (CombinationTree)
	{
		CombinationTree->DrawFromNode(ItemWidget);
	}
}

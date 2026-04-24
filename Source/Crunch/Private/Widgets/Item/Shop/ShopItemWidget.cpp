// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Item/Shop/ShopItemWidget.h"

#include "Components/ListView.h"
#include "CTypes/CStruct.h"
#include "FrameWork/CAssetManager.h"
#include "Inventory/Data/PA_ShopItem.h"

void UShopItemWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);
	InitWithShopItem(Cast<UPA_ShopItem>(ListItemObject));
	ParentListView = Cast<UListView>(IUserListEntry::GetOwningListView());
}

UUserWidget* UShopItemWidget::GetWidget() const
{
	UShopItemWidget* Copy = CreateWidget<UShopItemWidget>(GetOwningPlayer(), GetClass());
	Copy->CopyFromOther(this);
	return Copy;
}

TArray<const ITreeNodeInterface*> UShopItemWidget::GetInputs() const
{
	const FItemCollection* Collection = UCAssetManager::Get().GetCombinationForItem(GetShopItem());
	if (Collection)
	{
		return ItemsToInterfaces(Collection->GetItems());
	}

	return TArray<const ITreeNodeInterface*>();
}

TArray<const ITreeNodeInterface*> UShopItemWidget::GetOutputs() const
{
	const FItemCollection* Collection = UCAssetManager::Get().GetIngredientForItem(GetShopItem());
	if (Collection)
	{
		return ItemsToInterfaces(Collection->GetItems());
	}

	return TArray<const ITreeNodeInterface*>();
}

const UObject* UShopItemWidget::GetItemObject() const
{
	return ShopItem;
}

void UShopItemWidget::RightButtonClicked()
{
	OnShopItemPurchaseIssued.Broadcast(GetShopItem());
}

void UShopItemWidget::LeftButtonClicked()
{
	OnShopItemSelected.Broadcast(this);
}

void UShopItemWidget::CopyFromOther(const UShopItemWidget* OtherWidget)
{
	OnShopItemPurchaseIssued = OtherWidget->OnShopItemPurchaseIssued;
	OnShopItemSelected = OtherWidget->OnShopItemSelected;
	ParentListView = OtherWidget->ParentListView;
	InitWithShopItem(OtherWidget->GetShopItem());
}

void UShopItemWidget::InitWithShopItem(const UPA_ShopItem* NewShopItem)
{
	ShopItem = NewShopItem;
	if (!ShopItem)
	{
		return;
	}

	SetIcon(ShopItem->GetIcon());
	SetToolTipWidget(ShopItem);
}

TArray<const ITreeNodeInterface*> UShopItemWidget::ItemsToInterfaces(const TArray<const UPA_ShopItem*>& Items) const
{
	TArray<const ITreeNodeInterface*> RetInterfaces;
	if (!ParentListView)
	{
		return RetInterfaces;
	}

	for (const UPA_ShopItem* Item : Items)
	{
		const UShopItemWidget* ItemWidget = ParentListView->GetEntryWidgetFromItem<UShopItemWidget>(Item);
		if (ItemWidget)
		{
			RetInterfaces.Add(ItemWidget);
		}
	}

	return RetInterfaces;
}

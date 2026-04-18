// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Item/ItemToolTip.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Inventory/Data/PA_ShopItem.h"

void UItemToolTip::SetItem(const UPA_ShopItem* Item)
{
	ItemTitleText->SetText(Item->GetItemName());
	ItemDescriptionText->SetText(Item->GetItemDescription());
	ItemPriceText->SetText(FText::AsNumber(Item->GetPrice()));
	IconImage->SetBrushFromTexture(Item->GetIcon());
}

void UItemToolTip::SetPrice(float NewPrice)
{
	ItemPriceText->SetText(FText::AsNumber(NewPrice));
}

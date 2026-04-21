// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Item/Inventory/InventoryContextMenuWidget.h"

#include "Components/Button.h"

FOnButtonClickedEvent& UInventoryContextMenuWidget::GetUseButtonClickedEvent() const
{
	return UseButton->OnClicked;
}

FOnButtonClickedEvent& UInventoryContextMenuWidget::GetSellButtonClickedEvent() const
{
	return SellButton->OnClicked;
}

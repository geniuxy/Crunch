// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Item/Inventory/InventoryItemDragDropOp.h"
#include "Widgets/Item/Inventory/InventoryItemWidget.h"

void UInventoryItemDragDropOp::SetDraggedItem(UInventoryItemWidget* ClickedItemWidget)
{
	Payload = ClickedItemWidget;
	if (DragVisualClass)
	{
		UItemWidget* DragItemWidget = CreateWidget<UItemWidget>(GetWorld(), DragVisualClass);
		if (DragItemWidget)
		{
			DragItemWidget->SetIcon(ClickedItemWidget->GetIconTexture());
			DefaultDragVisual = DragItemWidget;
		}
	}
}

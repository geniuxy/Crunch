// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Item/Inventory/InventoryWidget.h"

#include "CrunchDebugHelper.h"
#include "Components/Button.h"
#include "Components/WrapBox.h"
#include "Components/WrapBoxSlot.h"
#include "Inventory/InventoryComponent.h"
#include "Widgets/Item/Inventory/InventoryContextMenuWidget.h"
#include "Widgets/Item/Inventory/InventoryItemWidget.h"

void UInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (APawn* OwnerPawn = GetOwningPlayerPawn())
	{
		InventoryComponent = OwnerPawn->GetComponentByClass<UInventoryComponent>();
		if (InventoryComponent)
		{
			InventoryComponent->OnItemAdded.AddUObject(this, &ThisClass::ItemAdded);
			InventoryComponent->OnItemRemoved.AddUObject(this, &ThisClass::ItemRemoved);
			InventoryComponent->OnItemStackCountChanged.AddUObject(this, &ThisClass::ItemStackCountChanged);
			int Capacity = InventoryComponent->GetCapacity();

			InventoryItemList->ClearChildren();

			for (int i = 0; i < Capacity; ++i)
			{
				UInventoryItemWidget* NewEmptyWidget =
					CreateWidget<UInventoryItemWidget>(GetOwningPlayer(), ItemWidgetClass);
				if (NewEmptyWidget)
				{
					NewEmptyWidget->SetSlotNumber(i);
					UWrapBoxSlot* NewItemSlot = InventoryItemList->AddChildToWrapBox(NewEmptyWidget);
					NewItemSlot->SetPadding(FMargin(2.f));
					InventoryItemWidgets.Add(NewEmptyWidget);

					NewEmptyWidget->OnInventoryItemDropped.AddUObject(this, &ThisClass::HandleItemDragDrop);
					NewEmptyWidget->OnLeftMouseClicked.AddUObject(InventoryComponent, &UInventoryComponent::TryActivateItem);
					NewEmptyWidget->OnRightMouseClicked.AddUObject(this, &ThisClass::ToggleContextMenu);
				}
			}

			SpawnContextMenu();
		}
	}
}

void UInventoryWidget::ItemAdded(const UInventoryItem* InventoryItem)
{
	if (!InventoryItem) return;

	if (UInventoryItemWidget* NextAvaliableSlot = GetNextAvaliableSlot())
	{
		NextAvaliableSlot->UpdateInventoryItem(InventoryItem);
		PopulatedInventoryItemWidgetsMap.Add(InventoryItem->GetHandle(), NextAvaliableSlot);
		if (InventoryComponent)
		{
			InventoryComponent->ItemSlotChanged(InventoryItem->GetHandle(), NextAvaliableSlot->GetSlotNumber());
		}
	}
}

void UInventoryWidget::ItemStackCountChanged(const FInventoryItemHandle& Handle, int NewCount)
{
	UInventoryItemWidget** FoundWidget = PopulatedInventoryItemWidgetsMap.Find(Handle);
	if (FoundWidget)
	{
		(*FoundWidget)->UpdateStackCount();
	}
}

UInventoryItemWidget* UInventoryWidget::GetNextAvaliableSlot() const
{
	for (UInventoryItemWidget* Widget : InventoryItemWidgets)
	{
		if (Widget->IsEmpty())
		{
			return Widget;
		}
	}

	return nullptr;
}

void UInventoryWidget::HandleItemDragDrop(UInventoryItemWidget* DestinationWidget, UInventoryItemWidget* SourceWidget)
{
	const UInventoryItem* SourceItem = SourceWidget->GetInventoryItem();
	const UInventoryItem* DestinationItem = DestinationWidget->GetInventoryItem();

	DestinationWidget->UpdateInventoryItem(SourceItem);
	SourceWidget->UpdateInventoryItem(DestinationItem);

	PopulatedInventoryItemWidgetsMap[DestinationWidget->GetItemHandle()] = DestinationWidget;
	if (InventoryComponent)
	{
		InventoryComponent->ItemSlotChanged(DestinationWidget->GetItemHandle(), DestinationWidget->GetSlotNumber());
	}

	if (!SourceWidget->IsEmpty())
	{
		PopulatedInventoryItemWidgetsMap[SourceWidget->GetItemHandle()] = SourceWidget;
		if (InventoryComponent)
		{
			InventoryComponent->ItemSlotChanged(SourceWidget->GetItemHandle(), SourceWidget->GetSlotNumber());
		}
	}
}

void UInventoryWidget::ItemRemoved(const FInventoryItemHandle& ItemHandle)
{
	UInventoryItemWidget** FoundWidget = PopulatedInventoryItemWidgetsMap.Find(ItemHandle);
	if (FoundWidget && *FoundWidget)
	{
		(*FoundWidget)->EmptySlot();
		PopulatedInventoryItemWidgetsMap.Remove(ItemHandle);
	}
}

void UInventoryWidget::SpawnContextMenu()
{
	if (!ContextMenuWidgetClass) return;

	ContextMenuWidget = CreateWidget<UInventoryContextMenuWidget>(this, ContextMenuWidgetClass);
	if (ContextMenuWidget)
	{
		ContextMenuWidget->GetSellButtonClickedEvent().AddDynamic(this, &ThisClass::SellFocusedItem);
		ContextMenuWidget->GetUseButtonClickedEvent().AddDynamic(this, &ThisClass::UseFocusedItem);
		ContextMenuWidget->AddToViewport(1);
		SetContextMenuVisible(false);
	}
}

void UInventoryWidget::SellFocusedItem()
{
	Debug::Print(TEXT("卖当前的Item"));
}

void UInventoryWidget::UseFocusedItem()
{
	Debug::Print(TEXT("使用当前的Item"));
}

void UInventoryWidget::SetContextMenuVisible(bool bContextMenuVisible)
{
	if (ContextMenuWidget)
	{
		ContextMenuWidget->SetVisibility(bContextMenuVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}
}

void UInventoryWidget::ToggleContextMenu(const FInventoryItemHandle& ItemHandle)
{
	Debug::Print(TEXT("打开物品内容菜单"));
}

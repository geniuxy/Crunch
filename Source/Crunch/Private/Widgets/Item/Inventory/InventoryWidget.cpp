// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Item/Inventory/InventoryWidget.h"

#include "Components/WrapBox.h"
#include "Components/WrapBoxSlot.h"
#include "Inventory/InventoryComponent.h"
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
				}
			}
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

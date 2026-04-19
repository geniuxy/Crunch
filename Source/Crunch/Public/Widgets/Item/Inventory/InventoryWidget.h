// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Inventory/InventoryItem.h"
#include "InventoryWidget.generated.h"

class UInventoryComponent;
class UInventoryItemWidget;
class UWrapBox;
/**
 * 
 */
UCLASS()
class CRUNCH_API UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

private:
	UPROPERTY(meta=(BindWidget))
	UWrapBox* InventoryItemList;

	UPROPERTY(EditDefaultsOnly, Category="Inventory")
	TSubclassOf<UInventoryItemWidget> ItemWidgetClass;

	UPROPERTY()
	UInventoryComponent* InventoryComponent;

	TArray<UInventoryItemWidget*> InventoryItemWidgets;
	TMap<FInventoryItemHandle, UInventoryItemWidget*> PopulatedInventoryItemWidgetsMap; // 已填充物品相关Map

	void ItemAdded(const UInventoryItem* InventoryItem);

	UInventoryItemWidget* GetNextAvaliableSlot() const;
};

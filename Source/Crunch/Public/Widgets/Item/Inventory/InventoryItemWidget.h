// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/Data/PA_ShopItem.h"
#include "Widgets/Item/ItemWidget.h"
#include "InventoryItemWidget.generated.h"

class UTextBlock;
class UInventoryItem;
/**
 * 
 */
UCLASS()
class CRUNCH_API UInventoryItemWidget : public UItemWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	bool IsEmpty() const;
	void UpdateInventoryItem(const UInventoryItem* Item);
	void EmptySlot();

private:
	void UpdateStackCount();

	UPROPERTY(EditDefaultsOnly, Category="Visual")
	UTexture2D* EmptyTexture;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* StackCountText;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* CooldownCountText;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* CooldownDurationText;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* ManaCostText;

	UPROPERTY()
	const UInventoryItem* InventoryItem;

	int SlotNumber;

public:
	FORCEINLINE int GetSlotNumber() const { return SlotNumber; }
	void SetSlotNumber(int NewSlotNumber) { SlotNumber = NewSlotNumber; }
};

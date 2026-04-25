// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/InventoryItem.h"
#include "Widgets/Item/ItemWidget.h"
#include "InventoryItemWidget.generated.h"

class UInventoryItemDragDropOp;
class UTextBlock;
class UInventoryItemWidget;

DECLARE_MULTICAST_DELEGATE_TwoParams(
	FOnInventoryItemDropped, UInventoryItemWidget* /* DestinationWidget*/, UInventoryItemWidget* /*SourceWidget*/
);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnButtonClicked, const FInventoryItemHandle& /* ClickedItemHandle */);
/**
 * 
 */
UCLASS()
class CRUNCH_API UInventoryItemWidget : public UItemWidget
{
	GENERATED_BODY()

public:
	FOnInventoryItemDropped OnInventoryItemDropped;
	FOnButtonClicked OnRightMouseClicked;
	FOnButtonClicked OnLeftMouseClicked;
	
	virtual void NativeConstruct() override;
	bool IsEmpty() const;
	void UpdateInventoryItem(const UInventoryItem* Item);
	void EmptySlot();
	void UpdateStackCount();

	UTexture2D* GetIconTexture() const;

	FInventoryItemHandle GetItemHandle() const;

private:
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

	virtual void RightButtonClicked() override;
	virtual void LeftButtonClicked() override;

public:
	FORCEINLINE int GetSlotNumber() const { return SlotNumber; }
	void SetSlotNumber(int NewSlotNumber) { SlotNumber = NewSlotNumber; }
	FORCEINLINE const UInventoryItem* GetInventoryItem() const { return InventoryItem; }

	/**********************************************************************/
	/*                            Drag Drop                               */
	/**********************************************************************/
private:
	virtual void NativeOnDragDetected(
		const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation
	) override;
	virtual bool NativeOnDrop(
		const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation
	) override;

	UPROPERTY(EditDefaultsOnly, Category="Drag Drop")
	TSubclassOf<UInventoryItemDragDropOp> DragDropOpClass;
	
	/**********************************************************************/
	/*                            Drag Drop                               */
	/**********************************************************************/
public:
	void StartCooldown(float CooldownDuration, float TimeRemaining);
};

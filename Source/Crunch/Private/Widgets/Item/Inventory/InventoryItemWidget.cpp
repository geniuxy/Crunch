// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Item/Inventory/InventoryItemWidget.h"

#include "CrunchDebugHelper.h"
#include "Blueprint/DragDropOperation.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Inventory/InventoryItem.h"
#include "Widgets/Item/ItemToolTip.h"
#include "Widgets/Item/Inventory/InventoryItemDragDropOp.h"

void UInventoryItemWidget::NativeConstruct()
{
	Super::NativeConstruct();
	EmptySlot();
}

bool UInventoryItemWidget::IsEmpty() const
{
	return !InventoryItem || !InventoryItem->IsValid();
}

void UInventoryItemWidget::UpdateInventoryItem(const UInventoryItem* Item)
{
	UnbindCanCastAbilityDelegate();
	
	InventoryItem = Item;
	if (!InventoryItem || !InventoryItem->IsValid() || InventoryItem->GetStackCount() <= 0)
	{
		EmptySlot();
		return;
	}

	SetIcon(Item->GetShopItem()->GetIcon());
	UItemToolTip* ToolTip = SetToolTipWidget(InventoryItem->GetShopItem());
	if (ToolTip)
	{
		ToolTip->SetPrice(InventoryItem->GetShopItem()->GetSellPrice());
	}

	if (InventoryItem->GetShopItem()->GetIsStackable())
	{
		StackCountText->SetVisibility(ESlateVisibility::Visible);
		UpdateStackCount();
	}
	else
	{
		StackCountText->SetVisibility(ESlateVisibility::Hidden);
	}

	ClearCooldown();

	if (InventoryItem->IsGrantingAnyAbility())
	{
		UpdateCanCastDisplay(InventoryItem->CanCastAbility());
		float AbilityCooldownRemaining = InventoryItem->GetAbilityCooldownTimerRemaining();
		float AbilityCooldownDuration = InventoryItem->GetAbilityCooldownDuration();
		if (AbilityCooldownRemaining > 0.f)
		{
			StartCooldown(AbilityCooldownDuration, AbilityCooldownRemaining);
		}

		float AbilityCost = InventoryItem->GetAbilityManaCost();
		ManaCostText->SetVisibility(AbilityCost == 0.f ? ESlateVisibility::Hidden : ESlateVisibility::Visible);
		ManaCostText->SetText(FText::AsNumber(AbilityCost));

		CooldownDurationText->SetVisibility(
			AbilityCooldownDuration == 0.f ? ESlateVisibility::Hidden : ESlateVisibility::Visible
		);
		CooldownDurationText->SetText(FText::AsNumber(AbilityCooldownDuration));
		BindCanCastAbilityDelegate();
	}
	else
	{
		UpdateCanCastDisplay(true);
		ManaCostText->SetVisibility(ESlateVisibility::Hidden);
		CooldownDurationText->SetVisibility(ESlateVisibility::Hidden);
		CooldownCountText->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UInventoryItemWidget::EmptySlot()
{
	ClearCooldown();
	UnbindCanCastAbilityDelegate();
	InventoryItem = nullptr;
	SetIcon(EmptyTexture);
	SetToolTip(nullptr);

	StackCountText->SetVisibility(ESlateVisibility::Hidden);
	CooldownCountText->SetVisibility(ESlateVisibility::Hidden);
	CooldownDurationText->SetVisibility(ESlateVisibility::Hidden);
	ManaCostText->SetVisibility(ESlateVisibility::Hidden);
}

void UInventoryItemWidget::UpdateStackCount()
{
	if (InventoryItem)
	{
		StackCountText->SetText(FText::AsNumber(InventoryItem->GetStackCount()));
	}
}

UTexture2D* UInventoryItemWidget::GetIconTexture() const
{
	if (InventoryItem && InventoryItem->GetShopItem())
	{
		return InventoryItem->GetShopItem()->GetIcon();
	}
	return nullptr;
}

FInventoryItemHandle UInventoryItemWidget::GetItemHandle() const
{
	if (!IsEmpty())
	{
		return InventoryItem->GetHandle();
	}

	return FInventoryItemHandle::InvalidHandle();
}

void UInventoryItemWidget::RightButtonClicked()
{
	if (!IsEmpty())
	{
		OnRightMouseClicked.Broadcast(GetItemHandle());
	}
}

void UInventoryItemWidget::LeftButtonClicked()
{
	if (!IsEmpty())
	{
		OnLeftMouseClicked.Broadcast(GetItemHandle());
	}
}

void UInventoryItemWidget::UpdateCanCastDisplay(bool bCanCast)
{
	ItemIcon->GetDynamicMaterial()->SetScalarParameterValue(CanCastDynamicMaterialParamName, bCanCast ? 1.f : 0.f);
}

void UInventoryItemWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent,
                                                UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

	if (!IsEmpty() && DragDropOpClass)
	{
		UInventoryItemDragDropOp* DragDropOp = NewObject<UInventoryItemDragDropOp>(this, DragDropOpClass);
		if (DragDropOp)
		{
			DragDropOp->SetDraggedItem(this);
			OutOperation = DragDropOp;
		}
	}
}

bool UInventoryItemWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
                                        UDragDropOperation* InOperation)
{
	if (UInventoryItemWidget* OtherWidget = Cast<UInventoryItemWidget>(InOperation->Payload))
	{
		if (OtherWidget && !OtherWidget->IsEmpty())
		{
			OnInventoryItemDropped.Broadcast(this, OtherWidget);
			return true;
		}
	}

	return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
}

void UInventoryItemWidget::StartCooldown(float CooldownDuration, float TimeRemaining)
{
	CooldownTimerRemaining = TimeRemaining;
	CooldownTimerDuration = CooldownDuration;
	GetWorld()->GetTimerManager().SetTimer(
		CooldownDurationHandle, this, &ThisClass::CooldownFinished, CooldownTimerRemaining
	);
	GetWorld()->GetTimerManager().SetTimer(
		CooldownUpdateTimerHandle, this, &ThisClass::UpdateCooldown, CooldownUpdateInterval, true
	);

	FNumberFormattingOptions CooldownDisplayFormattingOptions;
	CooldownDisplayFormattingOptions.MaximumFractionalDigits = CooldownTimerRemaining > 1.f ? 0 : 1;
	CooldownCountText->SetText(FText::AsNumber(CooldownTimerRemaining, &CooldownDisplayFormattingOptions));
	CooldownCountText->SetVisibility(ESlateVisibility::Visible);
}

void UInventoryItemWidget::BindCanCastAbilityDelegate()
{
	if (InventoryItem)
	{
		const_cast<UInventoryItem*>(InventoryItem)->OnAbilityCanCastUpdateDalegate.AddUObject(this, &ThisClass::UpdateCanCastDisplay);
	}
}

void UInventoryItemWidget::UnbindCanCastAbilityDelegate()
{
	if (InventoryItem)
	{
		const_cast<UInventoryItem*>(InventoryItem)->OnAbilityCanCastUpdateDalegate.RemoveAll(this);
	}
}

void UInventoryItemWidget::CooldownFinished()
{
	GetWorld()->GetTimerManager().ClearTimer(CooldownDurationHandle);
	GetWorld()->GetTimerManager().ClearTimer(CooldownUpdateTimerHandle);
	CooldownCountText->SetVisibility(ESlateVisibility::Hidden);
	if (ItemIcon)
	{
		ItemIcon->GetDynamicMaterial()->SetScalarParameterValue(CooldownAmtDynamicMaterialParamName, 1.f);
	}
	CooldownTimerRemaining = 0.f;
	CooldownTimerDuration = 0.f;
}

void UInventoryItemWidget::UpdateCooldown()
{
	CooldownTimerRemaining -= CooldownUpdateInterval;
	float CooldownAmt = 1.f - CooldownTimerRemaining / CooldownTimerDuration;
	FNumberFormattingOptions CooldownDisplayFormattingOptions;
	CooldownDisplayFormattingOptions.MaximumFractionalDigits = CooldownTimerRemaining > 1.f ? 0 : 1;
	CooldownCountText->SetText(FText::AsNumber(CooldownTimerRemaining, &CooldownDisplayFormattingOptions));
	if (ItemIcon)
	{
		ItemIcon->GetDynamicMaterial()->SetScalarParameterValue(CooldownAmtDynamicMaterialParamName, CooldownAmt);
	}
}

void UInventoryItemWidget::ClearCooldown()
{
	CooldownFinished();
}

void UInventoryItemWidget::SetIcon(UTexture2D* IconTexture)
{
	if (ItemIcon && ItemIcon->GetDynamicMaterial())
	{
		ItemIcon->GetDynamicMaterial()->SetTextureParameterValue(IconTextureDynamicMaterialParamName, IconTexture);
		return;
	}

	Super::SetIcon(IconTexture);
}

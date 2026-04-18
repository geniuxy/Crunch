// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Item/ItemWidget.h"

#include "CrunchDebugHelper.h"
#include "Components/Image.h"
#include "Widgets/Item/ItemToolTip.h"

void UItemWidget::NativeConstruct()
{
	Super::NativeConstruct();
	SetIsFocusable(true);
}

void UItemWidget::SetIcon(UTexture2D* IconTexture)
{
	ItemIcon->SetBrushFromTexture(IconTexture);
}

UItemToolTip* UItemWidget::SetToolTipWidget(const UPA_ShopItem* Item)
{
	if (!Item) return nullptr;

	if (GetOwningPlayer() && ItemToolTipClass)
	{
		UItemToolTip* ToolTip = CreateWidget<UItemToolTip>(GetOwningPlayer(), ItemToolTipClass);
		if (ToolTip)
		{
			ToolTip->SetItem(Item);
			SetToolTip(ToolTip);
		}
		
		return ToolTip;
	}

	return nullptr;
}

FReply UItemWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.IsMouseButtonDown(EKeys::RightMouseButton))
	{
		return FReply::Handled().SetUserFocus(TakeWidget());
	}

	if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
	{
		return FReply::Handled().SetUserFocus(TakeWidget()).DetectDrag(TakeWidget(), EKeys::LeftMouseButton);
	}

	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

FReply UItemWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (HasAnyUserFocus())
	{
		if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
		{
			RightButtonClicked();
			return FReply::Handled();
		}

		if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
		{
			LeftButtonClicked();
			return FReply::Handled();
		}
	}
	
	return Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);
}

void UItemWidget::RightButtonClicked()
{
	Debug::Print(TEXT("右键被点击了"));
}

void UItemWidget::LeftButtonClicked()
{
	Debug::Print(TEXT("左键被点击了"));
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Menu/WaitingWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"

void UWaitingWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

FOnButtonClickedEvent& UWaitingWidget::ClearAndGetButtonClickedEvent()
{
	CancelButton->OnClicked.Clear();
	return CancelButton->OnClicked;
}

void UWaitingWidget::SetWaitingInfo(const FText& WaitingInfo, bool bAllowCancel)
{
	if (CancelButton)
	{
		CancelButton->SetVisibility(bAllowCancel ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}

	if (WaitingInfoText)
	{
		WaitingInfoText->SetText(WaitingInfo);
	}
}

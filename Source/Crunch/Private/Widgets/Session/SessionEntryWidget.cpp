// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Session/SessionEntryWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"

void USessionEntryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SessionButton->OnClicked.AddDynamic(this, &ThisClass::SessionEntrySelected);
}

void USessionEntryWidget::InitializeEntry(const FString& Name, const FString& SessionIDStr)
{
	SessionNameText->SetText(FText::FromString(Name));
	CachedSessionIDStr = SessionIDStr;
}

void USessionEntryWidget::SessionEntrySelected()
{
	OnSessionEntrySelected.Broadcast(CachedSessionIDStr);
}

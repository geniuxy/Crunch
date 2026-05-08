// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Lobby/TeamSelectionWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"

void UTeamSelectionWidget::SetSlotID(uint8 NewSlotID)
{
	SlotID = NewSlotID;
}

void UTeamSelectionWidget::UpdateSlotInfo(const FString& PlayerNickName)
{
	InfoText->SetText(FText::FromString(PlayerNickName));
}

void UTeamSelectionWidget::NativeConstruct()
{
	Super::NativeConstruct();
	SelectionButton->OnClicked.AddDynamic(this, &ThisClass::SelectButtonClicked);
}

void UTeamSelectionWidget::SelectButtonClicked()
{
	OnSlotClicked.Broadcast(SlotID);
}

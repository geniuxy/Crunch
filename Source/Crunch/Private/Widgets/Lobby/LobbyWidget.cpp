// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Lobby/LobbyWidget.h"

#include "CrunchDebugHelper.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "FunctionLibrary/NetFunctionLibrary.h"
#include "Widgets/Lobby/TeamSelectionWidget.h"

void ULobbyWidget::NativeConstruct()
{
	Super::NativeConstruct();
	ClearAndPopulateTeamSelectionSlots();
}

void ULobbyWidget::ClearAndPopulateTeamSelectionSlots()
{
	TeamSelectionSlotGridPanel->ClearChildren();

	for (int i = 0; i < UNetFunctionLibrary::GetPlayerCountPerTeam() * 2; ++i)
	{
		UTeamSelectionWidget* NewSelectionSlot = CreateWidget<UTeamSelectionWidget>(this, TeamSelectionWidgetClass);
		if (NewSelectionSlot)
		{
			NewSelectionSlot->SetSlotID(i);
			UUniformGridSlot* NewGridSlot = TeamSelectionSlotGridPanel->AddChildToUniformGrid(NewSelectionSlot);
			if (NewGridSlot)
			{
				int Row = i % UNetFunctionLibrary::GetPlayerCountPerTeam();
				int Col = i / UNetFunctionLibrary::GetPlayerCountPerTeam();
				NewGridSlot->SetRow(Row);
				NewGridSlot->SetColumn(Col);
			}

			NewSelectionSlot->OnSlotClicked.AddUObject(this, &ThisClass::SlotSelected);
			TeamSelectionSlots.Add(NewSelectionSlot);
		}
	}
}

void ULobbyWidget::SlotSelected(uint8 NewSlotID)
{
	Debug::Print(FString::Printf(TEXT("尝试切换到槽位(%d)"), NewSlotID));
}

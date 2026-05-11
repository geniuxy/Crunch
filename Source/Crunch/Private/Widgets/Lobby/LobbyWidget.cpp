// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Lobby/LobbyWidget.h"

#include "CrunchDebugHelper.h"
#include "Actors/Lobby/CharacterDisplay.h"
#include "Characters/Data/PA_CharacterDefinition.h"
#include "Components/Button.h"
#include "Components/TileView.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "Components/WidgetSwitcher.h"
#include "Controllers/LobbyPlayerController.h"
#include "CTypes/PlayerInfoTypes.h"
#include "FrameWork/CAssetManager.h"
#include "FrameWork/CGameState.h"
#include "FunctionLibrary/NetFunctionLibrary.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "Player/CPlayerState.h"
#include "Widgets/Lobby/CharacterEntryWidget.h"
#include "Widgets/Lobby/TeamSelectionWidget.h"

void ULobbyWidget::NativeConstruct()
{
	Super::NativeConstruct();
	ClearAndPopulateTeamSelectionSlots();
	ConfigureGameState();
	LobbyPlayerController = GetOwningPlayer<ALobbyPlayerController>();
	if (LobbyPlayerController)
	{
		LobbyPlayerController->OnSwitchToHeroSelection.BindUObject(this, &ThisClass::SwitchToHeroSelection);
	}
	StartHeroSelectionButton->SetIsEnabled(false);
	StartHeroSelectionButton->OnClicked.AddDynamic(this, &ThisClass::StartHeroSelectionButtonClicked);

	UCAssetManager::Get().LoadCharacterDefinitions(
		FStreamableDelegate::CreateUObject(this, &ThisClass::CharacterDefinitionLoaded)
	);

	if (CharacterSelectionTileView)
	{
		// OnItemSelectionChanged()在取消选择时，不会做任何反应
		CharacterSelectionTileView->OnItemSelectionChanged().AddUObject(this, &ThisClass::CharacterSelected);
	}

	SpawnCharacterDisplay();
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
	if (LobbyPlayerController)
	{
		LobbyPlayerController->Server_RequestSlotSelectionChange(NewSlotID);
	}
}

void ULobbyWidget::ConfigureGameState()
{
	UWorld* World = GetWorld();
	if (!World) return;

	CGameState = World->GetGameState<ACGameState>();
	if (!CGameState)
	{
		World->GetTimerManager().SetTimer(ConfigureGameStateTimerHandle, this, &ThisClass::ConfigureGameState, 1.f);
	}
	else
	{
		CGameState->OnPlayerSelectionUpdated.AddUObject(this, &ThisClass::UpdatePlayerSelectionDisplay);
		UpdatePlayerSelectionDisplay(CGameState->GetPlayerSelection());
	}
}

void ULobbyWidget::UpdatePlayerSelectionDisplay(const TArray<FPlayerSelection>& PlayerSelections)
{
	for (UTeamSelectionWidget* SelectionSlot : TeamSelectionSlots)
	{
		SelectionSlot->UpdateSlotInfo(TEXT("-"));
	}

	for (UUserWidget* EntryWidget : CharacterSelectionTileView->GetDisplayedEntryWidgets())
	{
		if (UCharacterEntryWidget* CharacterEntryWidget = Cast<UCharacterEntryWidget>(EntryWidget))
		{
			CharacterEntryWidget->SetSelected(false);
		}
	}

	for (const FPlayerSelection& PlayerSelection : PlayerSelections)
	{
		if (!PlayerSelection.IsValid()) continue;

		TeamSelectionSlots[PlayerSelection.GetPlayerSlot()]->UpdateSlotInfo(PlayerSelection.GetPlayerNickName());

		UCharacterEntryWidget* SelectedEntry =
			CharacterSelectionTileView->GetEntryWidgetFromItem<UCharacterEntryWidget>(
				PlayerSelection.GetCharacterDefinition()
			);
		if (SelectedEntry)
		{
			SelectedEntry->SetSelected(true);
		}

		if (PlayerSelection.IsForPlayer(GetOwningPlayerState()))
		{
			UpdateCharacterDisplay(PlayerSelection);
		}
	}

	if (CGameState)
	{
		StartHeroSelectionButton->SetIsEnabled(CGameState->CanStartHeroSelection());
	}
}

void ULobbyWidget::StartHeroSelectionButtonClicked()
{
	if (LobbyPlayerController)
	{
		LobbyPlayerController->Server_StartHeroSelection();
	}
}

void ULobbyWidget::SwitchToHeroSelection()
{
	MainSwitcher->SetActiveWidget(HeroSelectionRoot);
}

void ULobbyWidget::CharacterDefinitionLoaded()
{
	TArray<UPA_CharacterDefinition*> LoadedCharacterDefinitions;
	if (UCAssetManager::Get().GetLoadedCharacterDefinitions(LoadedCharacterDefinitions))
	{
		CharacterSelectionTileView->SetListItems(LoadedCharacterDefinitions);
	}
}

void ULobbyWidget::CharacterSelected(UObject* SelectedUObject)
{
	if (!CPlayerState)
	{
		CPlayerState = GetOwningPlayerState<ACPlayerState>();
	}
	if (!CPlayerState) return;

	if (const UPA_CharacterDefinition* CharacterDefinition = Cast<UPA_CharacterDefinition>(SelectedUObject))
	{
		CPlayerState->Server_SetSelectedCharacterDefinition(CharacterDefinition);
	}
}

void ULobbyWidget::SpawnCharacterDisplay()
{
	if (CharacterDisplay) return;
	if (!CharacterDisplayClass) return;

	FTransform CharacterDisplayTransform = FTransform::Identity;
	AActor* PlayerStart = UGameplayStatics::GetActorOfClass(GetWorld(), APlayerStart::StaticClass());
	if (PlayerStart)
	{
		CharacterDisplayTransform = PlayerStart->GetActorTransform();
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	CharacterDisplay = GetWorld()->SpawnActor<ACharacterDisplay>(CharacterDisplayClass, SpawnParams);
	GetOwningPlayer()->SetViewTarget(CharacterDisplay);
}

void ULobbyWidget::UpdateCharacterDisplay(const FPlayerSelection& PlayerSelection)
{
	if (!PlayerSelection.GetCharacterDefinition()) return;

	CharacterDisplay->ConfigureWithCharacterDefinition(PlayerSelection.GetCharacterDefinition());
}

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LobbyWidget.generated.h"

class UTileView;
struct FPlayerSelection;
class ACGameState;
class ALobbyPlayerController;
class UTeamSelectionWidget;
class UUniformGridPanel;
class UButton;
class UWidgetSwitcher;
/**
 * 
 */
UCLASS()
class CRUNCH_API ULobbyWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

private:
	UPROPERTY(meta=(BindWidget))
	UWidgetSwitcher* MainSwitcher;

	UPROPERTY(meta=(BindWidget))
	UWidget* TeamSelectionRoot;

	UPROPERTY(meta=(BindWidget))
	UButton* StartHeroSelectionButton;

	UPROPERTY(meta=(BindWidget))
	UUniformGridPanel* TeamSelectionSlotGridPanel;

	UPROPERTY(meta=(BindWidget))
	UWidget* HeroSelectionRoot;

	UPROPERTY(meta=(BindWidget))
	UTileView* CharacterSelectionTileView;
	
	UPROPERTY(EditDefaultsOnly, Category="TeamSelection")
	TSubclassOf<UTeamSelectionWidget> TeamSelectionWidgetClass;

	UPROPERTY()
	TArray<UTeamSelectionWidget*> TeamSelectionSlots;

	void ClearAndPopulateTeamSelectionSlots();
	void SlotSelected(uint8 NewSlotID);

	UPROPERTY()
	ALobbyPlayerController* LobbyPlayerController;

	UPROPERTY()
	ACGameState* CGameState;
	
	void ConfigureGameState();

	FTimerHandle ConfigureGameStateTimerHandle;

	void UpdatePlayerSelectionDisplay(const TArray<FPlayerSelection>& PlayerSelections);
	
	UFUNCTION()
	void StartHeroSelectionButtonClicked();
	
	void SwitchToHeroSelection();
	
	void CharacterDefinitionLoaded();
};

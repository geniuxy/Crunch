// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/Data/PA_CharacterDefinition.h"
#include "GameFramework/GameStateBase.h"
#include "CGameState.generated.h"

class UPA_CharacterDefinition;
struct FPlayerSelection;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerSelectionUpdated, const TArray<FPlayerSelection>& /* NewPlayerSelection*/)
/**
 * 
 */
UCLASS()
class CRUNCH_API ACGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	FOnPlayerSelectionUpdated OnPlayerSelectionUpdated;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void RequestPlayerSelectionChange(const APlayerState* RequestingPlayer, uint8 DesiredSlot);
	bool IsSlotOccupied(uint8 SlotID) const;

	bool CanStartHeroSelection() const;
	bool CanStartMatch() const;

	bool IsDefinitionSelected(const UPA_CharacterDefinition* Definition) const;
	void SetCharacterSelected(const APlayerState* SelectingPlayer, const UPA_CharacterDefinition* SelectedDefinition);
	void SetCharacterDeselected(const UPA_CharacterDefinition* DefinitionToDeselect);

private:
	UPROPERTY(ReplicatedUsing=OnRep_PlayerSelectionArray)
	TArray<FPlayerSelection> PlayerSelectionArray;

	UFUNCTION()
	void OnRep_PlayerSelectionArray();

public:
	FORCEINLINE const TArray<FPlayerSelection>& GetPlayerSelection() const { return PlayerSelectionArray; }
};

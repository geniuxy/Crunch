// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "CGameState.generated.h"

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
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void RequestPlayerSelectionChange(const APlayerState* RequestingPlayer, uint8 DesiredSlot);
	bool IsSlotOccupied(uint8 SlotID) const;

	bool CanStartHeroSelection() const;

	FOnPlayerSelectionUpdated OnPlayerSelectionUpdated;

private:
	UPROPERTY(ReplicatedUsing=OnRep_PlayerSelectionArray)
	TArray<FPlayerSelection> PlayerSelectionArray;

	UFUNCTION()
	void OnRep_PlayerSelectionArray();

public:
	FORCEINLINE const TArray<FPlayerSelection>& GetPlayerSelection() const { return PlayerSelectionArray; }
};

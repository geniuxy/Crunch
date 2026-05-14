// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/CPlayerState.h"

#include "Characters/CCharacter.h"
#include "FrameWork/CGameState.h"
#include "FunctionLibrary/NetFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

ACPlayerState::ACPlayerState()
{
	bReplicates = true;
	NetUpdateFrequency = 100.f;
}

void ACPlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACPlayerState, PlayerSelection);
}

void ACPlayerState::BeginPlay()
{
	Super::BeginPlay();
	CGameState = Cast<ACGameState>(UGameplayStatics::GetGameState(this));
	if (CGameState)
	{
		CGameState->OnPlayerSelectionUpdated.AddUObject(this, &ThisClass::PlayerSelectionUpdated);
	}
}

void ACPlayerState::CopyProperties(APlayerState* PlayerState)
{
	// 在UE5.4中，如果想要正确执行CopyProperties，需要以下两种方法中的一种来执行
	// 1. StandAlone模式启动 2. 控制台输入net.AllowPIESeamlessTravel 1 （但是目前5.4有bug，会导致崩溃，可能之后的版本会修复）
	Super::CopyProperties(PlayerState);
	ACPlayerState* NewPlayerState = Cast<ACPlayerState>(PlayerState);
	if (NewPlayerState)
	{
		NewPlayerState->PlayerSelection = PlayerSelection;
	}
}

TSubclassOf<APawn> ACPlayerState::GetSelectedPawnClass() const
{
	if (PlayerSelection.GetCharacterDefinition())
	{
		return PlayerSelection.GetCharacterDefinition()->LoadCharacterClass();
	}

	return nullptr;
}

FGenericTeamId ACPlayerState::GetTeamIDBaseOnSlot() const
{
	return PlayerSelection.GetPlayerSlot() < UNetFunctionLibrary::GetPlayerCountPerTeam() ?
		FGenericTeamId(0) : FGenericTeamId(1);
}

void ACPlayerState::Server_SetSelectedCharacterDefinition_Implementation(const UPA_CharacterDefinition* NewDefinition)
{
	if (!CGameState || !NewDefinition) return;
	if (CGameState->IsDefinitionSelected(NewDefinition)) return;

	if (PlayerSelection.GetCharacterDefinition())
	{
		CGameState->SetCharacterDeselected(PlayerSelection.GetCharacterDefinition());
	}

	PlayerSelection.SetCharacterDefinition(NewDefinition);
	CGameState->SetCharacterSelected(this, NewDefinition);
}

bool ACPlayerState::Server_SetSelectedCharacterDefinition_Validate(const UPA_CharacterDefinition* NewDefinition)
{
	return true;
}

void ACPlayerState::PlayerSelectionUpdated(const TArray<FPlayerSelection>& NewPlayerSelections)
{
	for (const FPlayerSelection& NewPlayerSelection : NewPlayerSelections)
	{
		if (NewPlayerSelection.IsForPlayer(this))
		{
			PlayerSelection = NewPlayerSelection;
		}
	}
}

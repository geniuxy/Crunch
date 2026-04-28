// Fill out your copyright notice in the Description page of Project Settings.


#include "Crunch/Public/Player/CPlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"
#include "Characters/CPlayerCharacter.h"
#include "Net/UnrealNetwork.h"
#include "Widgets/GameplayWidget.h"

void ACPlayerController::OnPossess(APawn* NewPawn)
{
	Super::OnPossess(NewPawn);

	OwningPlayerCharacter = Cast<ACPlayerCharacter>(NewPawn);
	if (OwningPlayerCharacter)
	{
		OwningPlayerCharacter->ServerSideInit();
		OwningPlayerCharacter->SetGenericTeamId(TeamID);
	}
}

void ACPlayerController::AcknowledgePossession(APawn* NewPawn)
{
	Super::AcknowledgePossession(NewPawn);

	OwningPlayerCharacter = Cast<ACPlayerCharacter>(NewPawn);
	if (OwningPlayerCharacter)
	{
		OwningPlayerCharacter->ClientSideInit();
		SpawnGameplayWidget();
	}
}

void ACPlayerController::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACPlayerController, TeamID);
}

void ACPlayerController::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
	TeamID = NewTeamID;
}

FGenericTeamId ACPlayerController::GetGenericTeamId() const
{
	return TeamID;
}

void ACPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputLocalPlayerSubsystem* InputSubsystem = GetLocalPlayer()->GetSubsystem<
		UEnhancedInputLocalPlayerSubsystem>();
	if (InputSubsystem)
	{
		InputSubsystem->RemoveMappingContext(UIInputMapping);
		InputSubsystem->AddMappingContext(UIInputMapping, 1);
	}

	UEnhancedInputComponent* EnhancedInputComp = Cast<UEnhancedInputComponent>(InputComponent);
	if (EnhancedInputComp)
	{
		EnhancedInputComp->BindAction(
			ShopToggleInputAction, ETriggerEvent::Triggered, this, &ACPlayerController::ToggleShop
		);
		EnhancedInputComp->BindAction(
			ToggleGameplayMenuAction, ETriggerEvent::Triggered, this, &ACPlayerController::ToggleGameplayMenu
		);
	}
}

void ACPlayerController::MatchFinished(AActor* ViewTarget, int WinningTeam)
{
	if (!HasAuthority()) return;

	OwningPlayerCharacter->DisableInput(this);
	Client_MatchFinished(ViewTarget, WinningTeam);
}

void ACPlayerController::Client_MatchFinished_Implementation(AActor* ViewTarget, int WinningTeam)
{
	SetViewTargetWithBlend(ViewTarget, MatchFinishViewBlendTimerDuration);
	FString WinLoseMsg = TEXT("你赢了！");
	if (GetGenericTeamId().GetId() != WinningTeam)
	{
		WinLoseMsg = TEXT("你输了=_=");
	}

	GameplayWidget->SetGameplayMenuTitle(WinLoseMsg);
	FTimerHandle ShowWinLoseStateTimerHandle;
	GetWorldTimerManager().SetTimer(
		ShowWinLoseStateTimerHandle, this, &ThisClass::ShowWinLoseState, MatchFinishViewBlendTimerDuration
	);
}

void ACPlayerController::SpawnGameplayWidget()
{
	if (!IsLocalPlayerController()) // 排除在Client端，Server的副本角色的情况
	{
		return;
	}

	GameplayWidget = CreateWidget<UGameplayWidget>(this, GameplayWidgetClass);
	if (GameplayWidget)
	{
		GameplayWidget->AddToViewport();
		GameplayWidget->ConfigureAbilities(OwningPlayerCharacter->GetAbilities());
	}
}

void ACPlayerController::ToggleShop()
{
	if (GameplayWidget)
	{
		GameplayWidget->ToggleShop();
	}
}

void ACPlayerController::ToggleGameplayMenu()
{
	if (GameplayWidget)
	{
		GameplayWidget->ToggleGameMenu();
	}
}

void ACPlayerController::ShowWinLoseState()
{
	if (GameplayWidget)
	{
		GameplayWidget->ShowGameplayMenu();
	}
}

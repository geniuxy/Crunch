// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "GameFramework/PlayerController.h"
#include "CPlayerController.generated.h"

class UInputAction;
class UInputMappingContext;
class UGameplayWidget;
class ACPlayerCharacter;
/**
 * 
 */
UCLASS()
class CRUNCH_API ACPlayerController : public APlayerController, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	// 只在Server上执行
	virtual void OnPossess(APawn* NewPawn) override;
	// 只在Client上执行,也会在Listen Server(P2P 没有玩家当主机)上执行(其中一个玩家同时充当"服务器"角色)
	virtual void AcknowledgePossession(APawn* NewPawn) override;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	//~ Begin IGenericTeamAgentInterface Interface
	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;
	virtual FGenericTeamId GetGenericTeamId() const override;
	//~ End IGenericTeamAgentInterface Interface

	virtual void SetupInputComponent() override;

	void MatchFinished(AActor* ViewTarget, int WinningTeam);

protected:
	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<UGameplayWidget> GameplayWidgetClass;

	UPROPERTY()
	UGameplayWidget* GameplayWidget;
	
	UPROPERTY()
	TObjectPtr<ACPlayerCharacter> OwningPlayerCharacter;

	UPROPERTY(Replicated)
	FGenericTeamId TeamID;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	UInputMappingContext* UIInputMapping;
	
	UPROPERTY(EditDefaultsOnly, Category="Input")
	UInputAction* ShopToggleInputAction;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	UInputAction* ToggleGameplayMenuAction;
	
	UPROPERTY(EditDefaultsOnly, Category="View")
	float MatchFinishViewBlendTimerDuration = 2.f;

	UFUNCTION(Client, Reliable)
	void Client_MatchFinished(AActor* ViewTarget, int WinningTeam);
	
	void SpawnGameplayWidget();

	UFUNCTION()
	void ToggleShop();

	UFUNCTION()
	void ToggleGameplayMenu();

	void ShowWinLoseState();
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CPlayerController.generated.h"

class UGameplayWidget;
class ACPlayerCharacter;
/**
 * 
 */
UCLASS()
class CRUNCH_API ACPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	// 只在Server上执行
	virtual void OnPossess(APawn* NewPawn) override;
	// 只在Client上执行,也会在Listen Server(P2P 没有玩家当主机)上执行(其中一个玩家同时充当"服务器"角色)
	virtual void AcknowledgePossession(APawn* NewPawn) override;

protected:
	void SpawnGameplayWidget();

	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<UGameplayWidget> GameplayWidgetClass;

	UPROPERTY()
	UGameplayWidget* GameplayWidget;
	
	UPROPERTY()
	TObjectPtr<ACPlayerCharacter> OwningPlayerCharacter;
};

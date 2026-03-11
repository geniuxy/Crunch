// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CPlayerController.generated.h"

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
	// 只在Client上执行
	virtual void AcknowledgePossession(APawn* NewPawn) override;

protected:
	UPROPERTY()
	TObjectPtr<ACPlayerCharacter> OwningPlayerCharacter;
};

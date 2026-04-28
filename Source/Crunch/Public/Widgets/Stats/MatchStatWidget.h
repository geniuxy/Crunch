// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MatchStatWidget.generated.h"

class ACStormCore;
class UTextBlock;
class UImage;
/**
 * 
 */
UCLASS()
class CRUNCH_API UMatchStatWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

private:
	UPROPERTY(meta=(BindWidget))
	UImage* ProgressImage;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* TeamOneCountText;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* TeamTwoCountText;

	UPROPERTY()
	ACStormCore* StormCore;

	void UpdateTeamInfluence(int TeamOneCount, int TeamTwoCount);

	void MatchFinished(AActor* ViewTarget, int WinningTeam);
};

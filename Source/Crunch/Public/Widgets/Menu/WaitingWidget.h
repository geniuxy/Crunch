// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WaitingWidget.generated.h"

class UButton;
class UTextBlock;
class FOnButtonClickedEvent;
/**
 * 
 */
UCLASS()
class CRUNCH_API UWaitingWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	FOnButtonClickedEvent& ClearAndGetButtonClickedEvent();

	void SetWaitingInfo(const FText& WaitingInfo, bool bAllowCancel = false);

private:
	UPROPERTY(meta=(BindWidget))
	UTextBlock* WaitingInfoText;
	
	UPROPERTY(meta=(BindWidget))
	UButton* CancelButton;
};

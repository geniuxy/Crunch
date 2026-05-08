// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TeamSelectionWidget.generated.h"

class UTextBlock;
class UButton;
DECLARE_MULTICAST_DELEGATE_OneParam(FOnSlotClicked, uint8 /* SlotID */)
/**
 * 
 */
UCLASS()
class CRUNCH_API UTeamSelectionWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetSlotID(uint8 NewSlotID);
	void UpdateSlotInfo(const FString& PlayerNickName);
	virtual void NativeConstruct() override;

	FOnSlotClicked OnSlotClicked;

private:
	UPROPERTY(meta=(BindWidget))
	UButton* SelectionButton;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* InfoText;

	UFUNCTION()
	void SelectButtonClicked();

	uint8 SlotID;
};

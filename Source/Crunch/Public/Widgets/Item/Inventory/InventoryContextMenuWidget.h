// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryContextMenuWidget.generated.h"

class UButton;
class FOnButtonClickedEvent;
/**
 * 
 */
UCLASS()
class CRUNCH_API UInventoryContextMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	FOnButtonClickedEvent& GetUseButtonClickedEvent() const;
	FOnButtonClickedEvent& GetSellButtonClickedEvent() const;

private:
	UPROPERTY(meta=(BindWidget))
	UButton* UseButton;

	UPROPERTY(meta=(BindWidget))
	UButton* SellButton;
};

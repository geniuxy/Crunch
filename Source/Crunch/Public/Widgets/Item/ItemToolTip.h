// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemToolTip.generated.h"

class UTextBlock;
class UImage;
class UPA_ShopItem;
/**
 * 
 */
UCLASS()
class CRUNCH_API UItemToolTip : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetItem(const UPA_ShopItem* Item);
	void SetPrice(float NewPrice);

private:
	UPROPERTY(meta=(BindWidget))
	UImage* IconImage;
	
	UPROPERTY(meta=(BindWidget))
	UTextBlock* ItemTitleText;
	
	UPROPERTY(meta=(BindWidget))
	UTextBlock* ItemDescriptionText;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* ItemPriceText;
};

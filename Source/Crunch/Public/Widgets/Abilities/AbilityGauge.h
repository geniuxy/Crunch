// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Blueprint/UserWidget.h"
#include "AbilityGauge.generated.h"

struct FAbilityWidgetData;
class UTextBlock;
class UImage;
/**
 * 
 */
UCLASS()
class CRUNCH_API UAbilityGauge : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

public:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

	void ConfigureWithWidgetData(const FAbilityWidgetData* WidgetData);

private:
	UPROPERTY(meta=(BindWidget))
	UImage* Icon;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* CoolDownCounterText;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* CoolDownDurationText;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* CostText;

	UPROPERTY(EditDefaultsOnly, Category="Visual")
	FName IconMaterialParamName = "Icon";
};

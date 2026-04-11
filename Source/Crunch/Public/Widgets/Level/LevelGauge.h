// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LevelGauge.generated.h"

struct FOnAttributeChangeData;
class UAbilitySystemComponent;
class UTextBlock;
class UImage;
/**
 * 
 */
UCLASS()
class CRUNCH_API ULevelGauge : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

private:
	UPROPERTY(EditDefaultsOnly, Category="Visual")
	FName PercentMaterialParamName = "Percent";

	UPROPERTY(meta=(BindWidget))
	UImage* LevelProgressImage;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* LevelText;

	UPROPERTY()
	UAbilitySystemComponent* OwnerASC;

	void UpdateGauge(const FOnAttributeChangeData& Data);
};

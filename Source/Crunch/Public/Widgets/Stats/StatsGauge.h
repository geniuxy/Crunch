// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "Blueprint/UserWidget.h"
#include "StatsGauge.generated.h"

struct FOnAttributeChangeData;
class UTextBlock;
class UImage;
/**
 * 
 */
UCLASS()
class CRUNCH_API UStatsGauge : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;

private:
	UPROPERTY(meta=(BindWidget))
	UImage* Icon;
	
	UPROPERTY(meta=(BindWidget))
	UTextBlock* AttributeText;

	UPROPERTY(EditAnywhere, Category="Attribute")
	FGameplayAttribute Attribute;

	UPROPERTY(EditAnywhere, Category="Visual")
	UTexture2D* IconTexture;

	void SetValue(float NewVal);

	void AttributeChanged(const FOnAttributeChangeData& Data);
};

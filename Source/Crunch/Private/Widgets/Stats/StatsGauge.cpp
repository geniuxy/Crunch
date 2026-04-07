// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Stats/StatsGauge.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "GAS/CAttributeSet.h"

void UStatsGauge::NativePreConstruct()
{
	Super::NativePreConstruct();
	Icon->SetBrushFromTexture(IconTexture);
}

void UStatsGauge::NativeConstruct()
{
	Super::NativeConstruct();

	APawn* OwnerPlayerPawn = GetOwningPlayerPawn();
	if (!OwnerPlayerPawn) return;

	UAbilitySystemComponent* OwnerASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OwnerPlayerPawn);
	if (OwnerASC)
	{
		bool bFound;
		float AttributeVal = OwnerASC->GetGameplayAttributeValue(Attribute, bFound);
		SetValue(AttributeVal);

		OwnerASC->GetGameplayAttributeValueChangeDelegate(Attribute).AddUObject(this, &ThisClass::AttributeChanged);
	}
}

void UStatsGauge::SetValue(float NewVal)
{
	FNumberFormattingOptions NumberFormattingOptions;
	NumberFormattingOptions.MaximumFractionalDigits = 0;

	AttributeText->SetText(FText::AsNumber(NewVal, &NumberFormattingOptions));
}

void UStatsGauge::AttributeChanged(const FOnAttributeChangeData& Data)
{
	SetValue(Data.NewValue);
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/ValueGauge.h"

#include "CrunchDebugHelper.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UValueGauge::NativePreConstruct()
{
	Super::NativePreConstruct();

	ProgressBar->SetFillColorAndOpacity(BarColor);
}

void UValueGauge::SetValue(float NewValue, float NewMaxValue)
{
	if (NewMaxValue == 0)
	{
		Debug::Print(FString::Printf(TEXT("Value Gauge : %s, NewMaxValue can't be 0"), *GetName()));
		return;
	}

	float NewPercent = NewValue / NewMaxValue;
	ProgressBar->SetPercent(NewPercent);

	FNumberFormattingOptions FormatOps = FNumberFormattingOptions().SetMaximumFractionalDigits(0);

	ValueText->SetText(
		FText::Format(
			FTextFormat::FromString("{0}/{1}"),
			FText::AsNumber(NewValue, &FormatOps),
			FText::AsNumber(NewMaxValue, &FormatOps)
		)
	);
}

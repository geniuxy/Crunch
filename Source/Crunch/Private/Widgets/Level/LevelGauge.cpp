// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Level/LevelGauge.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "GAS/CHeroAttributeSet.h"

void ULevelGauge::NativeConstruct()
{
	Super::NativeConstruct();

	APawn* OwnerPawn = GetOwningPlayerPawn();
	if (!OwnerPawn) return;

	OwnerASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OwnerPawn);
	if (!OwnerASC) return;

	UpdateGauge(FOnAttributeChangeData());
	OwnerASC->GetGameplayAttributeValueChangeDelegate(UCHeroAttributeSet::GetExperienceAttribute()).AddUObject(
		this, &ThisClass::UpdateGauge
	);
	OwnerASC->GetGameplayAttributeValueChangeDelegate(UCHeroAttributeSet::GetPrevLevelExperienceAttribute()).AddUObject(
		this, &ThisClass::UpdateGauge
	);
	OwnerASC->GetGameplayAttributeValueChangeDelegate(UCHeroAttributeSet::GetNextLevelExperienceAttribute()).AddUObject(
		this, &ThisClass::UpdateGauge
	);
	OwnerASC->GetGameplayAttributeValueChangeDelegate(UCHeroAttributeSet::GetLevelAttribute()).AddUObject(
		this, &ThisClass::UpdateGauge
	);
}

void ULevelGauge::UpdateGauge(const FOnAttributeChangeData& Data)
{
	bool bFound;
	float CurrentExperience = OwnerASC->GetGameplayAttributeValue(
		UCHeroAttributeSet::GetExperienceAttribute(), bFound
	);
	if (!bFound) return;
	float NextLevelExperience = OwnerASC->GetGameplayAttributeValue(
		UCHeroAttributeSet::GetNextLevelExperienceAttribute(), bFound
	);
	if (!bFound) return;
	float PrevLevelExperience = OwnerASC->GetGameplayAttributeValue(
		UCHeroAttributeSet::GetPrevLevelExperienceAttribute(), bFound
	);
	if (!bFound) return;
	float CurrentLevel = OwnerASC->GetGameplayAttributeValue(
		UCHeroAttributeSet::GetLevelAttribute(), bFound
	);
	if (!bFound) return;

	FNumberFormattingOptions NumberFormattingOptions;
	NumberFormattingOptions.SetMaximumFractionalDigits(0);
	LevelText->SetText(FText::AsNumber(CurrentLevel, &NumberFormattingOptions));

	float Progress = CurrentExperience - PrevLevelExperience;
	float LevelExpAmt= NextLevelExperience - PrevLevelExperience;
	float Percent = Progress / LevelExpAmt;

	if (NextLevelExperience == 0) // 满级之后，NextLevelExperience等于0
	{
		Percent = 1;
	}

	if (LevelProgressImage)
	{
		LevelProgressImage->GetDynamicMaterial()->SetScalarParameterValue(PercentMaterialParamName, Percent);
	}
}

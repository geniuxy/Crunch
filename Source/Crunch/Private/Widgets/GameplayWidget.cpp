// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/GameplayWidget.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "GAS/CAttributeSet.h"
#include "Widgets/ValueGauge.h"
#include "Widgets/Abilities/AbilityListView.h"

void UGameplayWidget::NativeConstruct()
{
	Super::NativeConstruct();

	OwnerAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwningPlayerPawn());
	if (OwnerAbilitySystemComponent)
	{
		HealthBar->SetAndBoundToGameplayAttribute(
			OwnerAbilitySystemComponent,
			UCAttributeSet::GetHealthAttribute(),
			UCAttributeSet::GetMaxHealthAttribute()
		);

		ManaBar->SetAndBoundToGameplayAttribute(
			OwnerAbilitySystemComponent,
			UCAttributeSet::GetManaAttribute(),
			UCAttributeSet::GetMaxManaAttribute()
		);
	}
}

void UGameplayWidget::ConfigureAbilities(const TMap<ECAbilityInputID, TSubclassOf<UGameplayAbility>>& Abilities)
{
	AbilityListView->ConfigureAbilities(Abilities);
}

void UGameplayWidget::ToggleShop()
{
	if (ShopWidget->GetVisibility() == ESlateVisibility::HitTestInvisible)
	{
		ShopWidget->SetVisibility(ESlateVisibility::Visible);
		PlayShopPopUpAnimation(true);
	}
	else
	{
		ShopWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
		PlayShopPopUpAnimation(false);
	}
}

void UGameplayWidget::PlayShopPopUpAnimation(bool bPlayForward)
{
	if (bPlayForward)
	{
		PlayAnimationForward(ShopPopUpAnimation);
	}
	else
	{
		PlayAnimationReverse(ShopPopUpAnimation);
	}
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Crosshair/CrosshairWidget.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "CGameplayTags.h"
#include "CrunchDebugHelper.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"

void UCrosshairWidget::NativeConstruct()
{
	Super::NativeConstruct();

	CrosshairImage->SetVisibility(ESlateVisibility::Hidden);

	UAbilitySystemComponent* OwnerASC =
		UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwningPlayerPawn());
	if (OwnerASC)
	{
		OwnerASC->RegisterGameplayTagEvent(CGameplayTags::Crunch_Stats_Crosshair).AddUObject(
			this, &ThisClass::CrosshairTagUpdated);
	}

	CachedPlayerController = GetOwningPlayer();

	CrosshairCanvasPanelSlot = Cast<UCanvasPanelSlot>(Slot);
	if (!CrosshairCanvasPanelSlot)
	{
		Debug::Print(TEXT("CrosshairWidget需要CanvasPanel作为父Widget"));
	}
}

void UCrosshairWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (CrosshairImage->GetVisibility() == ESlateVisibility::Visible)
	{
		UpdateCrosshairPosition();
	}
}

void UCrosshairWidget::CrosshairTagUpdated(const FGameplayTag Tag, int32 NewCount)
{
	CrosshairImage->SetVisibility(NewCount > 0 ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
}

void UCrosshairWidget::UpdateCrosshairPosition()
{
	if (!CachedPlayerController || !CrosshairCanvasPanelSlot) return;

	float ViewportScale = UWidgetLayoutLibrary::GetViewportScale(this);
	int32 SizeX, SizeY;
	CachedPlayerController->GetViewportSize(SizeX, SizeY);
	FVector2D ViewportSize = FVector2D((float)SizeX, (float)SizeY);
	CrosshairCanvasPanelSlot->SetPosition(ViewportSize / ViewportScale / 2.f);
}

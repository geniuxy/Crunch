// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Crosshair/CrosshairWidget.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "CGameplayTags.h"
#include "CrunchDebugHelper.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "GAS/CAbilitySystemComponent.h"

void UCrosshairWidget::NativeConstruct()
{
	Super::NativeConstruct();

	CrosshairImage->SetVisibility(ESlateVisibility::Hidden);

	UAbilitySystemComponent* OwnerASC =
		UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwningPlayerPawn());
	if (OwnerASC)
	{
		OwnerASC->RegisterGameplayTagEvent(CGameplayTags::Crunch_Stats_Crosshair).AddUObject(
			this, &ThisClass::CrosshairTagUpdated
		);
		OwnerASC->GenericGameplayEventCallbacks.Add(CGameplayTags::Crunch_Event_Target_Updated).AddUObject(
			this, &ThisClass::TargetUpdated
		);
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

	UCAbilitySystemComponent* OwnerASC = Cast<UCAbilitySystemComponent>(
		UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwningPlayerPawn())
	);
	AActor* CurrentAimTarget = OwnerASC->GetAimTarget();
	if (!CurrentAimTarget)
	{
		FVector2D ViewportSize = FVector2D((float)SizeX, (float)SizeY);
		CrosshairCanvasPanelSlot->SetPosition(ViewportSize / ViewportScale / 2.f);
		return;
	}

	FVector2D TargetScreenPosition;
	CachedPlayerController->ProjectWorldLocationToScreen(CurrentAimTarget->GetActorLocation(), TargetScreenPosition);
	if (TargetScreenPosition.X > 0 && TargetScreenPosition.X < SizeX &&
		TargetScreenPosition.Y > 0 && TargetScreenPosition.Y < SizeY)
	{
		CrosshairCanvasPanelSlot->SetPosition(TargetScreenPosition / ViewportScale);
	}
	else
	{
		// 当镜头不包含目标时，清空锁定目标
		if (OwnerASC)
		{
			OwnerASC->ClearAimTarget();
		}
	}
}

void UCrosshairWidget::TargetUpdated(const FGameplayEventData* EventData)
{
	const AActor* AimTarget = EventData->Target;
	CrosshairImage->SetColorAndOpacity(AimTarget ? HasTargetColor : NoTargetColor);
}

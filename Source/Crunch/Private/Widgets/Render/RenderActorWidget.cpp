// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Render/RenderActorWidget.h"

#include "CrunchDebugHelper.h"
#include "Actors/Render/RenderActor.h"
#include "Components/Image.h"
#include "Components/SizeBox.h"
#include "Engine/TextureRenderTarget2D.h"

void URenderActorWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	RenderSizeBox->SetWidthOverride(RenderSize.X);
	RenderSizeBox->SetHeightOverride(RenderSize.Y);
}

void URenderActorWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SpawnRenderActor();
	ConfigureRenderActor();
	BeginRenderCapture();
}

void URenderActorWidget::BeginDestroy()
{
	StopRenderCapture();
	Super::BeginDestroy();
}

void URenderActorWidget::ConfigureRenderActor()
{
	if (!GetRenderActor())
	{
		Debug::Print(TEXT("没有捕获到RenderActor，没有东西会被渲染"));
		return;
	}

	RenderTarget = NewObject<UTextureRenderTarget2D>(this);
	RenderTarget->InitAutoFormat((int)RenderSize.X, (int)RenderSize.Y);
	RenderTarget->RenderTargetFormat = ETextureRenderTargetFormat::RTF_RGBA8_SRGB;
	GetRenderActor()->SetRenderTarget(RenderTarget);

	UMaterialInstanceDynamic* DisplayImageDynamicMaterial = DisplayImage->GetDynamicMaterial();
	if (DisplayImageDynamicMaterial)
	{
		DisplayImageDynamicMaterial->SetTextureParameterValue(DisplayImageRenderTargetParamName, RenderTarget);
	}
}

void URenderActorWidget::BeginRenderCapture()
{
	RenderTickInterval = 1.f / (float)FrameRate;
	UWorld* World = GetWorld();
	if (World)
	{
		World->GetTimerManager().SetTimer(RenderTimerHandle, this, &ThisClass::UpdateRender, RenderTickInterval, true);
	}
}

void URenderActorWidget::UpdateRender()
{
	if (GetRenderActor())
	{
		GetRenderActor()->UpdateRender();
	}
}

void URenderActorWidget::StopRenderCapture()
{
	UWorld* World = GetWorld();
	if (World)
	{
		World->GetTimerManager().ClearTimer(RenderTimerHandle);
	}
}

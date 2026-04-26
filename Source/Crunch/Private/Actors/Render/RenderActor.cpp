// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Render/RenderActor.h"

#include "Components/SceneCaptureComponent2D.h"


ARenderActor::ARenderActor()
{
	PrimaryActorTick.bCanEverTick = false;

	RootComp = CreateDefaultSubobject<USceneComponent>("RootComp");
	SetRootComponent(RootComp);

	CaptureComponent = CreateDefaultSubobject<USceneCaptureComponent2D>("CaptureComponent");
	CaptureComponent->SetupAttachment(RootComp);
	CaptureComponent->bCaptureEveryFrame = false;
	CaptureComponent->FOVAngle = 30.f;
}

void ARenderActor::SetRenderTarget(UTextureRenderTarget2D* RenderTarget)
{
	CaptureComponent->TextureTarget = RenderTarget;
}

void ARenderActor::UpdateRender()
{
	if (CaptureComponent)
	{
		CaptureComponent->CaptureScene();
	}
}

void ARenderActor::BeginPlay()
{
	Super::BeginPlay();

	CaptureComponent->ShowOnlyActorComponents(this);
}


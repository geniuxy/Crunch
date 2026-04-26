// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RenderActor.generated.h"

UCLASS()
class CRUNCH_API ARenderActor : public AActor
{
	GENERATED_BODY()

public:
	ARenderActor();

	void SetRenderTarget(UTextureRenderTarget2D* RenderTarget);
	void UpdateRender();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleDefaultsOnly, Category="Render Actor")
	USceneComponent* RootComp;

	UPROPERTY(VisibleDefaultsOnly, Category="Render Actor")
	USceneCaptureComponent2D* CaptureComponent;
};

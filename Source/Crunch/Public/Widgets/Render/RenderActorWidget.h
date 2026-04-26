// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RenderActorWidget.generated.h"

class USizeBox;
class UImage;
class ARenderActor;
/**
 * 
 */
UCLASS(Abstract)
class CRUNCH_API URenderActorWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual void BeginDestroy() override;

private:
	void ConfigureRenderActor();
	// 因为 UE 的 UObject 反射系统要求每个 UClass 都能被实例化（用于 CDO、序列化、蓝图反射等），所以不能存在真正的抽象类
	// PURE_VIRTUAL让类在 C++ 编译器眼中不是抽象类，满足了 UE 的构造需求；
	// 1) 非 UObject 的普通 C++ 类	直接用 = 0
	// 2) UObject / UCLASS 类	必须用 PURE_VIRTUAL
	// 3) UE 接口（UInterface）	通常用 PURE_VIRTUAL
	virtual void SpawnRenderActor() PURE_VIRTUAL(URenderActorWidget::SpawnRenderActor,);
	virtual ARenderActor* GetRenderActor() const PURE_VIRTUAL(URenderActorWidget::GetRenderActor, return nullptr;);

	void BeginRenderCapture();
	void UpdateRender();
	void StopRenderCapture();

	UPROPERTY(meta=(BindWidget))
	UImage* DisplayImage;

	UPROPERTY(meta=(BindWidget))
	USizeBox* RenderSizeBox;

	UPROPERTY(EditDefaultsOnly, Category="Render Actor")
	FName DisplayImageRenderTargetParamName = "RenderTarget";

	UPROPERTY(EditDefaultsOnly, Category="Render Actor")
	FVector2D RenderSize;

	UPROPERTY(EditDefaultsOnly, Category="Render Actor")
	int FrameRate = 24;

	float RenderTickInterval;
	FTimerHandle RenderTimerHandle;

	UPROPERTY()
	UTextureRenderTarget2D* RenderTarget;
};

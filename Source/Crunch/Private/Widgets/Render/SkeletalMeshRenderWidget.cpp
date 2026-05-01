// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Render/SkeletalMeshRenderWidget.h"

#include "Actors/Render/SkeletalMeshRenderActor.h"
#include "Components/SceneCaptureComponent2D.h"
#include "GameFramework/Character.h"
#include "Widgets/Render/RenderActorTargetInterface.h"

void USkeletalMeshRenderWidget::NativeConstruct()
{
	Super::NativeConstruct();
	ACharacter* OwnerPlayerCharacter = GetOwningPlayerPawn<ACharacter>();
	IRenderActorTargetInterface* RenderTargetInterface = Cast<IRenderActorTargetInterface>(OwnerPlayerCharacter);
	if (OwnerPlayerCharacter && SkeletalMeshRenderActor)
	{
		SkeletalMeshRenderActor->ConfigureSkeletalMesh(
			OwnerPlayerCharacter->GetMesh()->GetSkeletalMeshAsset(), OwnerPlayerCharacter->GetMesh()->GetAnimClass()
		);
		USceneCaptureComponent2D* SceneCaptureComponent = SkeletalMeshRenderActor->GetCaptureComponent();
		if (RenderTargetInterface && SceneCaptureComponent)
		{
			SceneCaptureComponent->SetRelativeLocation(RenderTargetInterface->GetCaptureLocalPosition());
			SceneCaptureComponent->SetRelativeRotation(RenderTargetInterface->GetCaptureLocalRotation());
		}
	}
}

void USkeletalMeshRenderWidget::SpawnRenderActor()
{
	if (!SkeletalMeshRenderActorClass) return;

	UWorld* World = GetWorld();
	if (!World) return;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	SkeletalMeshRenderActor = World->SpawnActor<ASkeletalMeshRenderActor>(SkeletalMeshRenderActorClass, SpawnParams);
}

ARenderActor* USkeletalMeshRenderWidget::GetRenderActor() const
{
	return SkeletalMeshRenderActor;
}

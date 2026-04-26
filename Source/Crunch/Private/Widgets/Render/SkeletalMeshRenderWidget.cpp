// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Render/SkeletalMeshRenderWidget.h"

#include "Actors/Render/SkeletalMeshRenderActor.h"
#include "GameFramework/Character.h"

void USkeletalMeshRenderWidget::NativeConstruct()
{
	Super::NativeConstruct();
	ACharacter* OwnerPlayerCharacter = GetOwningPlayerPawn<ACharacter>();
	if (OwnerPlayerCharacter && SkeletalMeshRenderActor)
	{
		SkeletalMeshRenderActor->ConfigureSkeletalMesh(
			OwnerPlayerCharacter->GetMesh()->GetSkeletalMeshAsset(), OwnerPlayerCharacter->GetMesh()->GetAnimClass()
		);
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

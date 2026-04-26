// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Render/SkeletalMeshRenderActor.h"


ASkeletalMeshRenderActor::ASkeletalMeshRenderActor()
{
	MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>("SkeletalMeshComp");
	MeshComponent->SetupAttachment(GetRootComponent());
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshComponent->SetLightingChannels(false, true, false); // 预览所用的Mesh光照使用独立的第二个通道，与不受默认的第一通道干扰
}

void ASkeletalMeshRenderActor::ConfigureSkeletalMesh(USkeletalMesh* MeshAsset, TSubclassOf<UAnimInstance> AnimBlueprint)
{
	MeshComponent->SetSkeletalMeshAsset(MeshAsset);
	MeshComponent->SetAnimInstanceClass(AnimBlueprint);
}

void ASkeletalMeshRenderActor::BeginPlay()
{
	Super::BeginPlay();
	MeshComponent->SetVisibleInSceneCaptureOnly(true);
}

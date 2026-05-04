// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/TargetActors/TargetActor_Line.h"

#include "NiagaraComponent.h"
#include "Components/SphereComponent.h"


ATargetActor_Line::ATargetActor_Line()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	ShouldProduceTargetDataOnServer = true;

	RootComp = CreateDefaultSubobject<USceneComponent>("RootComp");
	SetRootComponent(RootComp);

	TargetEndDetectionSphere = CreateDefaultSubobject<USphereComponent>("TargetEndDetectionSphere");
	TargetEndDetectionSphere->SetupAttachment(GetRootComponent());
	TargetEndDetectionSphere->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	LaserVFX = CreateDefaultSubobject<UNiagaraComponent>("LaserVFX");
	LaserVFX->SetupAttachment(GetRootComponent());

	AvatarActor = nullptr;
}


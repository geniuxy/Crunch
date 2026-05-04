// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/TargetActors/TargetActor_Line.h"

#include "NiagaraComponent.h"
#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"


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

void ATargetActor_Line::ConfigureTargetSetting(
	float NewTargetRange,
	float NewDetectionCylinderRadius,
	float NewTargetingInterval,
	FGenericTeamId OwnerTeamID,
	bool bShouldDrawDebug)
{
	TargetRange = NewTargetRange;
	DetectionCylinderRadius = NewDetectionCylinderRadius;
	TargetingInterval = NewTargetingInterval;
	SetGenericTeamId(OwnerTeamID);
	bDrawDebug = bShouldDrawDebug;
}

void ATargetActor_Line::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATargetActor_Line, TeamID);
	DOREPLIFETIME(ATargetActor_Line, TargetRange);
	DOREPLIFETIME(ATargetActor_Line, DetectionCylinderRadius);
	DOREPLIFETIME(ATargetActor_Line, AvatarActor);
}


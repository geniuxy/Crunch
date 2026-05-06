// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/TargetActors/TargetActor_BlackHole.h"

#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"
#include "Particles/ParticleSystemComponent.h"


ATargetActor_BlackHole::ATargetActor_BlackHole()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	ShouldProduceTargetDataOnServer = true;

	RootComp = CreateDefaultSubobject<USceneComponent>("RootComp");
	SetRootComponent(RootComp);

	DetectionSphereComponent = CreateDefaultSubobject<USphereComponent>("DetectSphereComponent");
	DetectionSphereComponent->SetupAttachment(GetRootComponent());
	DetectionSphereComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	DetectionSphereComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	DetectionSphereComponent->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::ActorInBlackHoleRange);
	DetectionSphereComponent->OnComponentEndOverlap.AddDynamic(this, &ThisClass::ActorLeftBlackHoleRange);

	VFXComponent = CreateDefaultSubobject<UParticleSystemComponent>("VFXComponent");
	VFXComponent->SetupAttachment(GetRootComponent());
}

void ATargetActor_BlackHole::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ATargetActor_BlackHole, TeamID);
	DOREPLIFETIME_CONDITION_NOTIFY(ATargetActor_BlackHole, BlackHoleRange, COND_None, REPNOTIFY_Always);
}

void ATargetActor_BlackHole::ConfigureBlackHole(
	float InBlackHoleRange, float InPullSpeed, float InBlackHoleDuration, const FGenericTeamId& InTeamID)
{
	PullSpeed= InPullSpeed;
	
	BlackHoleRange = InBlackHoleRange;
	DetectionSphereComponent->SetSphereRadius(InBlackHoleRange);

	SetGenericTeamId(InTeamID);

	BlackHoleDuration = InBlackHoleDuration;
}

void ATargetActor_BlackHole::OnRep_BlackHoleRange()
{
	DetectionSphereComponent->SetSphereRadius(BlackHoleRange);
}

void ATargetActor_BlackHole::ActorInBlackHoleRange(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
}

void ATargetActor_BlackHole::ActorLeftBlackHoleRange(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int OtherBodyIndex)
{
}


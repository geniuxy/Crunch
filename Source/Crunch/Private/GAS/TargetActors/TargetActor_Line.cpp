// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/TargetActors/TargetActor_Line.h"

#include "NiagaraComponent.h"
#include "Abilities/GameplayAbility.h"
#include "Components/SphereComponent.h"
#include "Kismet/KismetMathLibrary.h"
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

void ATargetActor_Line::StartTargeting(UGameplayAbility* Ability)
{
	Super::StartTargeting(Ability);
	if (!OwningAbility) return;

	AvatarActor = OwningAbility->GetAvatarActorFromActorInfo();
	if (HasAuthority())
	{
		GetWorldTimerManager().SetTimer(
			PeriodicalTargetingTimerHandle, this, &ThisClass::DoTargetCheckAndReport, TargetingInterval, true
		);
	}
}

void ATargetActor_Line::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	UpdateTargetTrace();
}

void ATargetActor_Line::DoTargetCheckAndReport()
{
}

void ATargetActor_Line::UpdateTargetTrace()
{
	FVector ViewLocation = GetActorLocation();
	FRotator ViewRotation = GetActorRotation();
	if (AvatarActor)
	{
		AvatarActor->GetActorEyesViewPoint(ViewLocation, ViewRotation);
	}

	FVector LookEndPoint = ViewLocation + ViewRotation.Vector() * 1000000;
	FRotator LookRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), LookEndPoint);
	SetActorRotation(LookRotation);

	FVector SweepEndLocation = GetActorLocation() + LookRotation.Vector() * TargetRange;

	TArray<FHitResult> HitResults;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(AvatarActor);
	QueryParams.AddIgnoredActor(this);

	FCollisionResponseParams CollisionResponseParams(ECR_Overlap);

	GetWorld()->SweepMultiByChannel(
		HitResults,
		GetActorLocation(),
		SweepEndLocation,
		FQuat::Identity,
		ECC_WorldDynamic,
		FCollisionShape::MakeSphere(DetectionCylinderRadius),
		QueryParams,
		CollisionResponseParams
	);

	FVector LineEndLocation = SweepEndLocation;
	float LineLength = TargetRange;

	for (FHitResult& HitResult : HitResults)
	{
		if (HitResult.GetActor())
		{
			if (GetTeamAttitudeTowards(*HitResult.GetActor()) != ETeamAttitude::Friendly)
			{
				LineEndLocation = HitResult.ImpactPoint;
				LineLength = FVector::Distance(GetActorLocation(), LineEndLocation);
				break;
			}
		}
	}

	TargetEndDetectionSphere->SetWorldLocation(LineEndLocation);
	if (LaserVFX)
	{
		LaserVFX->SetVariableFloat(LaserFXLengthParamName, LineLength / 100.f); // LineLength的单位是cm，ue中需要的是m
	}
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Projectiles/ProjectileActor.h"

#include "Net/UnrealNetwork.h"


AProjectileActor::AProjectileActor()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true; // 开启了这个 Actor 的网络复制通道，不会自动复制所有属性

	USceneComponent* RootComp = CreateDefaultSubobject<USceneComponent>("RootComp");
	SetRootComponent(RootComp);
}

void AProjectileActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (HasAuthority())
	{
		if (Target)
		{
			MoveDir = (Target->GetActorLocation() - GetActorLocation()).GetSafeNormal();
		}
	}

	SetActorLocation(GetActorLocation() + MoveDir * ProjectileSpeed * DeltaSeconds);
}

void AProjectileActor::ShootProjectile(
	float InSpeed,
	float InMaxDistance,
	const AActor* InTarget,
	FGenericTeamId InTeamID,
	FGameplayEffectSpecHandle InHitEffectHandle)
{
	Target = InTarget;
	ProjectileSpeed = InSpeed;

	FRotator OwnerViewRot = GetActorRotation();
	SetGenericTeamId(InTeamID);
	if (GetOwner())
	{
		FVector OwnerViewLoc;
		GetOwner()->GetActorEyesViewPoint(OwnerViewLoc, OwnerViewRot);
	}
	MoveDir = OwnerViewRot.Vector();

	HitEffectSpecHandle = InHitEffectHandle;

	float TravelMaxTime = InMaxDistance / InSpeed;
	GetWorldTimerManager().SetTimer(ShootTimerHandle, this, &ThisClass::TravelMaxDistanceReached, TravelMaxTime);
}

void AProjectileActor::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AProjectileActor, MoveDir);
	DOREPLIFETIME(AProjectileActor, TeamID);
	DOREPLIFETIME(AProjectileActor, ProjectileSpeed);
}

void AProjectileActor::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
	TeamID = NewTeamID;
}

FGenericTeamId AProjectileActor::GetGenericTeamId() const
{
	return TeamID;
}

void AProjectileActor::TravelMaxDistanceReached()
{
	Destroy();
}

void AProjectileActor::BeginPlay()
{
	Super::BeginPlay();
}

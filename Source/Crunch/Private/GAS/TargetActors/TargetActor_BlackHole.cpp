// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/TargetActors/TargetActor_BlackHole.h"

#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
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
	PullSpeed = InPullSpeed;

	BlackHoleRange = InBlackHoleRange;
	DetectionSphereComponent->SetSphereRadius(InBlackHoleRange);

	SetGenericTeamId(InTeamID);

	BlackHoleDuration = InBlackHoleDuration;
}

void ATargetActor_BlackHole::SetBlackHoleLinkOrigin()
{
	if (HasAuthority())
	{
		for (TPair<AActor*, UNiagaraComponent*>& TargetPair : ActorsInRangeMap)
		{
			UNiagaraComponent* NiagaraComponent = TargetPair.Value;
			if (NiagaraComponent)
			{
				NiagaraComponent->SetVariablePosition(
					BlackHoleVFXOriginVariableName, VFXComponent->GetComponentLocation()
				);
			}
		}
	}
}

void ATargetActor_BlackHole::StartTargeting(UGameplayAbility* Ability)
{
	Super::StartTargeting(Ability);

	UWorld* World = GetWorld();
	if (World)
	{
		World->GetTimerManager().SetTimer(
			BlackHoleDurationTimerHandle, this, &ThisClass::StopBlackHole, BlackHoleDuration
		);
	}
}

void ATargetActor_BlackHole::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (HasAuthority())
	{
		for (TPair<AActor*, UNiagaraComponent*>& TargetPair : ActorsInRangeMap)
		{
			AActor* Target = TargetPair.Key;

			FVector PullDir = GetActorLocation() - Target->GetActorLocation();
			PullDir.Z = 0;
			PullDir.Normalize();
			Target->SetActorLocation(Target->GetActorLocation() + PullDir * PullSpeed * DeltaSeconds);
		}
	}
}

void ATargetActor_BlackHole::ConfirmTargetingAndContinue()
{
	StopBlackHole();
}

void ATargetActor_BlackHole::CancelTargeting()
{
	StopBlackHole();
	Super::CancelTargeting();
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
	TryAddTarget(OtherActor);
}

void ATargetActor_BlackHole::ActorLeftBlackHoleRange(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int OtherBodyIndex)
{
	RemoveTarget(OtherActor);
}

void ATargetActor_BlackHole::TryAddTarget(AActor* OtherTarget)
{
	if (!OtherTarget || ActorsInRangeMap.Contains(OtherTarget)) return;
	if (GetTeamAttitudeTowards(*OtherTarget) != ETeamAttitude::Hostile) return;

	UNiagaraComponent* NiagaraComponent = nullptr;
	if (BlackHoleLinkVFX)
	{
		NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
			BlackHoleLinkVFX, OtherTarget->GetRootComponent(), NAME_None, FVector::Zero(), FRotator::ZeroRotator,
			EAttachLocation::KeepRelativeOffset, false
		);
		if (NiagaraComponent)
		{
			NiagaraComponent->SetVariablePosition(
				BlackHoleVFXOriginVariableName, VFXComponent->GetComponentLocation()
			);
		}
	}

	ActorsInRangeMap.Add(OtherTarget, NiagaraComponent);
}

void ATargetActor_BlackHole::RemoveTarget(AActor* OtherTarget)
{
	if (!OtherTarget) return;

	if (ActorsInRangeMap.Contains(OtherTarget))
	{
		UNiagaraComponent* VFXComp;
		ActorsInRangeMap.RemoveAndCopyValue(OtherTarget, VFXComp);
		if (IsValid(VFXComp))
		{
			VFXComp->DestroyComponent();
		}
	}
}

void ATargetActor_BlackHole::StopBlackHole()
{
	TArray<TWeakObjectPtr<AActor>> FinalTargets;
	for (TPair<AActor*, UNiagaraComponent*>& TargetPair : ActorsInRangeMap)
	{
		FinalTargets.Add(TargetPair.Key);
		UNiagaraComponent* NiagaraComponent = TargetPair.Value;
		if (IsValid(NiagaraComponent))
		{
			NiagaraComponent->DestroyComponent();
		}
	}

	FGameplayAbilityTargetDataHandle TargetDataHandle;

	FGameplayAbilityTargetData_ActorArray* TargetActorArray = new FGameplayAbilityTargetData_ActorArray;
	TargetActorArray->SetActors(FinalTargets);
	TargetDataHandle.Add(TargetActorArray);

	FGameplayAbilityTargetData_SingleTargetHit* BlowUpLocation = new FGameplayAbilityTargetData_SingleTargetHit;
	BlowUpLocation->HitResult.ImpactPoint = GetActorLocation();
	TargetDataHandle.Add(BlowUpLocation);

	TargetDataReadyDelegate.Broadcast(TargetDataHandle);
}

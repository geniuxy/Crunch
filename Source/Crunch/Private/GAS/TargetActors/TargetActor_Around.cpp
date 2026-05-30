// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/TargetActors/TargetActor_Around.h"

#include "Abilities/GameplayAbility.h"
#include "Components/SphereComponent.h"
#include "FunctionLibrary/CAbilitySystemFunctionLibrary.h"
#include "Net/UnrealNetwork.h"


ATargetActor_Around::ATargetActor_Around()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	ShouldProduceTargetDataOnServer = true;

	RootComp = CreateDefaultSubobject<USceneComponent>("Root Comp");
	SetRootComponent(RootComp);

	DetectionSphere = CreateDefaultSubobject<USphereComponent>("Detection Sphere");
	DetectionSphere->SetupAttachment(GetRootComponent());
	DetectionSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	DetectionSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	DetectionSphere->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::ActorInDetectionRange);
}

void ATargetActor_Around::ConfigureDetection(
	float DetectionRadius, const FGenericTeamId& InTeamID, const FGameplayTag& InLocalGameplayCueTag)
{
	SetGenericTeamId(InTeamID);
	DetectionSphere->SetSphereRadius(DetectionRadius);
	TargetDetectionRadius = DetectionRadius;
	LocalGameplayCueTag = InLocalGameplayCueTag;
}

void ATargetActor_Around::SetGenericTeamId(const FGenericTeamId& InTeamID)
{
	TeamID = InTeamID;
}

void ATargetActor_Around::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ATargetActor_Around, TeamID);
	DOREPLIFETIME(ATargetActor_Around, LocalGameplayCueTag);
	DOREPLIFETIME_CONDITION_NOTIFY(ATargetActor_Around, TargetDetectionRadius, COND_None, REPNOTIFY_Always);
}

void ATargetActor_Around::OnRep_TargetDetectionRadiusReplicated()
{
	DetectionSphere->SetSphereRadius(TargetDetectionRadius);
}

void ATargetActor_Around::ActorInDetectionRange(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (!OtherActor) return;
	AActor* AvatarActor = nullptr;
	if (OwningAbility)
	{
		AvatarActor = OwningAbility->GetAvatarActorFromActorInfo();
	}
	if (OtherActor == AvatarActor) return;
	if (OtherActor == this) return;
	if (GetTeamAttitudeTowards(*OtherActor) != ETeamAttitude::Hostile) return;

	if (HasAuthority())
	{
		FGameplayAbilityTargetDataHandle TargetDataHandle;
		/** Adds a new target data to handle, it must have been created with new */
		FGameplayAbilityTargetData_ActorArray* ActorArray = new FGameplayAbilityTargetData_ActorArray;
		ActorArray->SetActors(TArray<TWeakObjectPtr<AActor>>{OtherActor});

		TargetDataHandle.Add(ActorArray);
		TargetDataReadyDelegate.Broadcast(TargetDataHandle);
	}

	FHitResult HitResult;
	HitResult.ImpactPoint = OtherActor->GetActorLocation();
	HitResult.ImpactNormal = (OtherActor->GetActorLocation() - GetActorLocation()).GetSafeNormal();
	UCAbilitySystemFunctionLibrary::SendLocalGameplayCue(OtherActor, HitResult, LocalGameplayCueTag);
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/CStormCore.h"

#include "AIController.h"
#include "CrunchDebugHelper.h"
#include "GenericTeamAgentInterface.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"


ACStormCore::ACStormCore()
{
	PrimaryActorTick.bCanEverTick = true;

	InfluenceRange = CreateDefaultSubobject<USphereComponent>("InfluenceRange");
	InfluenceRange->SetupAttachment(GetRootComponent());

	InfluenceRange->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::NewInfluencerInRange);
	InfluenceRange->OnComponentEndOverlap.AddDynamic(this, &ThisClass::InfluencerLeftRange);

	ViewCam = CreateDefaultSubobject<UCameraComponent>("ViewCam");
	ViewCam->SetupAttachment(GetRootComponent());

	GroundDecalComponent = CreateDefaultSubobject<UDecalComponent>("GroundDecalComponent");
	GroundDecalComponent->SetupAttachment(GetRootComponent());
}

float ACStormCore::GetProgress() const
{
	FVector TeamTwoGoalLoc = TeamTwoGoal->GetActorLocation();
	FVector VectorFromTeamOne = GetActorLocation() - TeamTwoGoalLoc;
	VectorFromTeamOne.Z = 0.f;
	return VectorFromTeamOne.Length() / TravelLength;
}

void ACStormCore::BeginPlay()
{
	Super::BeginPlay();

	FVector TeamOneGoalLoc = TeamOneGoal->GetActorLocation();
	FVector TeamTwoGoalLoc = TeamTwoGoal->GetActorLocation();
	FVector GoalOffset = TeamOneGoalLoc - TeamTwoGoalLoc;
	GoalOffset.Z = 0.f;
	TravelLength = GoalOffset.Length();
}

void ACStormCore::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	OwnerAIController = Cast<AAIController>(NewController);
}

void ACStormCore::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(ACStormCore, CoreToCapture, COND_None, REPNOTIFY_Always);
}

void ACStormCore::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (CoreToCapture)
	{
		FVector CoreMoveDir = (GetMesh()->GetComponentLocation() - CoreToCapture->GetActorLocation()).GetSafeNormal();
		CoreToCapture->AddActorWorldOffset(CoreMoveDir * CoreCaptureSpeed * DeltaSeconds);
	}
}

void ACStormCore::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	
	FName PropertyName = PropertyChangedEvent.GetPropertyName();
	if (PropertyName == GET_MEMBER_NAME_CHECKED(ACStormCore, InfluenceRadius))
	{
		InfluenceRange->SetSphereRadius(InfluenceRadius);
		FVector DecalSize = GroundDecalComponent->DecalSize;
		GroundDecalComponent->DecalSize = FVector(DecalSize.X, InfluenceRadius, InfluenceRadius);
	}
}

void ACStormCore::NewInfluencerInRange(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (OtherActor == TeamOneGoal)
	{
		GoalReached(0);
	}
	if (OtherActor == TeamTwoGoal)
	{
		GoalReached(1);
	}
	
	IGenericTeamAgentInterface* OtherTeamInterface = Cast<IGenericTeamAgentInterface>(OtherActor);
	if (OtherTeamInterface)
	{
		if (OtherTeamInterface->GetGenericTeamId().GetId() == 0)
		{
			TeamOneInfluencerCount++;
		}
		else if (OtherTeamInterface->GetGenericTeamId().GetId() == 1)
		{
			TeamTwoInfluencerCount++;
		}
		UpdateTeamWeight();
	}
}

void ACStormCore::InfluencerLeftRange(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	IGenericTeamAgentInterface* OtherTeamInterface = Cast<IGenericTeamAgentInterface>(OtherActor);
	if (OtherTeamInterface)
	{
		if (OtherTeamInterface->GetGenericTeamId().GetId() == 0)
		{
			TeamOneInfluencerCount--;
			if (TeamOneInfluencerCount < 0)
			{
				TeamOneInfluencerCount = 0;
			}
		}
		else if (OtherTeamInterface->GetGenericTeamId().GetId() == 1)
		{
			TeamTwoInfluencerCount--;
			if (TeamTwoInfluencerCount < 0)
			{
				TeamTwoInfluencerCount = 0;
			}
		}
		UpdateTeamWeight();
	}
}

void ACStormCore::UpdateTeamWeight()
{
	OnTeamInfluencerCountUpdatedDelegate.Broadcast(TeamOneInfluencerCount, TeamTwoInfluencerCount);
	if (TeamOneInfluencerCount == TeamTwoInfluencerCount)
	{
		TeamWeight = 0;
	}
	else
	{
		float TeamOffset = TeamOneInfluencerCount - TeamTwoInfluencerCount;
		float TeamTotal = TeamOneInfluencerCount + TeamTwoInfluencerCount;

		TeamWeight = TeamOffset / TeamTotal;
	}
	UpdateGoal();
}

void ACStormCore::UpdateGoal()
{
	if (!HasAuthority()) return;
	if (!OwnerAIController) return;
	if (!GetCharacterMovement()) return;

	if (TeamWeight > 0.f)
	{
		OwnerAIController->MoveToActor(TeamOneGoal);
	}
	else
	{
		OwnerAIController->MoveToActor(TeamTwoGoal);
	}

	float Speed = MaxWalkSpeed * FMath::Abs(TeamWeight);
	GetCharacterMovement()->MaxWalkSpeed = Speed;
}

void ACStormCore::OnRep_CoreToCapture()
{
	if (CoreToCapture)
	{
		CaptureCore();
	}
}

void ACStormCore::GoalReached(int WinningTeam)
{
	OnGoalReachedDelegate.Broadcast(this, WinningTeam);

	if (!HasAuthority()) return;

	CoreToCapture = WinningTeam == 0 ? TeamTwoCore : TeamOneCore;
	CaptureCore();
}

void ACStormCore::CaptureCore()
{
	float ExpandDuration = GetMesh()->GetAnimInstance()->Montage_Play(ExpandMontage);
	CoreCaptureSpeed =
		FVector::Distance(GetMesh()->GetComponentLocation(), CoreToCapture->GetActorLocation()) / ExpandDuration;

	CoreToCapture->SetActorEnableCollision(false);
	GetCharacterMovement()->MaxWalkSpeed = 0.f;

	FTimerHandle ExpandTimerHandle;
	GetWorldTimerManager().SetTimer(ExpandTimerHandle, this, &ThisClass::ExpandFinished, ExpandDuration);
}

void ACStormCore::ExpandFinished()
{
	CoreToCapture->SetActorLocation(GetMesh()->GetComponentLocation());
	// 败方的基地和StormCore Attach在一起，一起消失
	CoreToCapture->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepWorldTransform, "root");
	GetMesh()->GetAnimInstance()->Montage_Play(CaptureMontage);
}


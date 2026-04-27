// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/CStormCore.h"

#include "AIController.h"
#include "CrunchDebugHelper.h"
#include "GenericTeamAgentInterface.h"
#include "Components/SphereComponent.h"
#include "GameFramework/CharacterMovementComponent.h"


ACStormCore::ACStormCore()
{
	PrimaryActorTick.bCanEverTick = false;

	InfluenceRange = CreateDefaultSubobject<USphereComponent>("InfluenceRange");
	InfluenceRange->SetupAttachment(GetRootComponent());

	InfluenceRange->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::NewInfluencerInRange);
	InfluenceRange->OnComponentEndOverlap.AddDynamic(this, &ThisClass::InfluencerLeftRange);
}

void ACStormCore::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACStormCore::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	OwnerAIController = Cast<AAIController>(NewController);
}

void ACStormCore::NewInfluencerInRange(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
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

	Debug::Print(FString::Printf(TEXT("一队个数：%d, 二队个数：%d, 权重：%f"), TeamOneInfluencerCount, TeamTwoInfluencerCount, TeamWeight));
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


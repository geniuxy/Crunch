// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/CMinionBarrack.h"

#include "Characters/CMinion.h"
#include "GameFramework/PlayerStart.h"

void ACMinionBarrack::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		GetWorldTimerManager().SetTimer(SpawnIntervalTimerHandle, this, &ACMinionBarrack::SpawnNewGroup,
		                                GroupSpawnInterval, true);
	}
}

const APlayerStart* ACMinionBarrack::GetNextSpawnSpot()
{
	if (SpawnSpots.Num() == 0) return nullptr;

	++NextSpawnSpotIndex;

	if (NextSpawnSpotIndex >= SpawnSpots.Num())
	{
		NextSpawnSpotIndex = 0;
	}

	return SpawnSpots[NextSpawnSpotIndex];
}

void ACMinionBarrack::SpawnNewGroup()
{
	int i = MinionPerGroup;

	while (i > 0)
	{
		FTransform SpawnTransform = GetActorTransform();
		if (const APlayerStart* NextSpawnSpot = GetNextSpawnSpot())
		{
			SpawnTransform = NextSpawnSpot->GetActorTransform();
		}

		ACMinion* NextAvaliableMinion = GetNextAvaliableMinion();
		if (!NextAvaliableMinion) break;

		NextAvaliableMinion->SetActorTransform(SpawnTransform);
		NextAvaliableMinion->Activate();

		--i;
	}

	SpawnNewMinions(i);
}

void ACMinionBarrack::SpawnNewMinions(int Amount)
{
	for (int i = 0; i < Amount; ++i)
	{
		FTransform SpawnTransform = GetActorTransform();
		if (const APlayerStart* NextSpawnSpot = GetNextSpawnSpot())
		{
			SpawnTransform = NextSpawnSpot->GetActorTransform();
		}

		ACMinion* NewMinion = GetWorld()->SpawnActorDeferred<ACMinion>(
			MinionClass, SpawnTransform, this, nullptr,
			ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn
		);
		NewMinion->SetGenericTeamId(BarrackTeamID);
		// FinishSpawning后，会调用AIController的OnPossess，需要在那之前些给Pawn SetGenericTeamId
		NewMinion->FinishSpawning(SpawnTransform);
		// 这个MinionGoal需要bCanEverAffectNavigation(Can Ever Affect Navigation) = false
		NewMinion->SetGoal(MinionGoal);
		MinionPool.Add(NewMinion);
	}
}

ACMinion* ACMinionBarrack::GetNextAvaliableMinion() const
{
	for (ACMinion* Minion : MinionPool)
	{
		if (!Minion->IsActive())
		{
			return Minion;
		}
	}
	return nullptr;
}

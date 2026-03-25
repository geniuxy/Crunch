// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "GameFramework/Actor.h"
#include "CMinionBarrack.generated.h"


class ACMinion;

UCLASS()
class CRUNCH_API ACMinionBarrack : public AActor
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, Category="Spawn")
	FGenericTeamId BarrackTeamID;
	
	UPROPERTY(EditAnywhere, Category="Spawn")
	int MinionPerGroup = 3; // 每组需要生成的Minion个数
	
	UPROPERTY(EditAnywhere, Category="Spawn")
	float GroupSpawnInterval = 5.f;
	
	UPROPERTY(EditAnywhere, Category="Spawn")
	TSubclassOf<ACMinion> MinionClass;

	UPROPERTY(EditAnywhere, Category="Spawn")
	TArray<APlayerStart*> SpawnSpots;
	
	UPROPERTY()
	TArray<ACMinion*> MinionPool;

	int NextSpawnSpotIndex = -1;

	const APlayerStart* GetNextSpawnSpot();

	void SpawnNewGroup();
	void SpawnNewMinions(int Amount);
	ACMinion* GetNextAvaliableMinion() const;

	FTimerHandle SpawnIntervalTimerHandle;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CStormCore.generated.h"

class USphereComponent;

UCLASS()
class CRUNCH_API ACStormCore : public ACharacter
{
	GENERATED_BODY()

public:
	ACStormCore();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleDefaultsOnly, Category="Detection")
	USphereComponent* InfluenceRange;

	UFUNCTION()
	void NewInfluencerInRange(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	UFUNCTION()
	void InfluencerLeftRange(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);

	void UpdateTeamWeight();

	int TeamOneInfluencerCount = 0;
	int TeamTwoInfluencerCount = 0;

	float TeamWeight = 0.f;
};

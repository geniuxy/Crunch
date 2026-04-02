// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "TargetActor_GroundPick.generated.h"

/**
 * 
 */
UCLASS()
class CRUNCH_API ATargetActor_GroundPick : public AGameplayAbilityTargetActor
{
	GENERATED_BODY()

public:
	ATargetActor_GroundPick();

protected:
	virtual void Tick(float DeltaSeconds) override;

	FVector GetTargetPoint() const;

	UPROPERTY(EditDefaultsOnly, Category="Targetting")
	float TargetTraceRange = 2000.f;
};

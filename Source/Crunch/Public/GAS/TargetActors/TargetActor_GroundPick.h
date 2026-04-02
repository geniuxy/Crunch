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

	virtual void ConfirmTargetingAndContinue() override;
	void SetTargetOptions(bool bTargetFriendly, bool bTargetEnemy = true);

protected:
	bool bShouldTargetEnemy = true;
	bool bShouldTargetFriendly = false;

	virtual void Tick(float DeltaSeconds) override;

	FVector GetTargetPoint() const;

	UPROPERTY(EditDefaultsOnly, Category="Targeting")
	float TargetTraceRange = 2000.f;

	UPROPERTY(EditDefaultsOnly, Category="Targeting")
	float TargetAreaRadius = 300.f;

	bool bShouldDrawDebug = false;

public:
	FORCEINLINE void SetTargetAreaRadius(float NewRadius) { TargetAreaRadius = NewRadius; }
	FORCEINLINE void SetTargetTraceRange(float NewRange) { TargetTraceRange = NewRange; }
	FORCEINLINE void SetShouldDrawDebug(bool bDrawDebug) { bShouldDrawDebug = bDrawDebug; }
};

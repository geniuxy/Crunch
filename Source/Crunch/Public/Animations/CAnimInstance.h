// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CAnimInstance.generated.h"

class UCharacterMovementComponent;
/**
 * 
 */
UCLASS()
class CRUNCH_API UCAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;

	UFUNCTION(BlueprintPure, meta=(BlueprintThreadSafe))
	FORCEINLINE float GetSpeed() const { return Speed; }

	UFUNCTION(BlueprintPure, meta=(BlueprintThreadSafe))
	FORCEINLINE bool IsMoving() const { return Speed != 0.f; }

private:
	UPROPERTY()
	ACharacter* OwnerPlayerCharacter;

	UPROPERTY()
	UCharacterMovementComponent* OwnerMovementComp;

	float Speed;
};

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

	UFUNCTION(BlueprintPure, meta=(BlueprintThreadSafe))
	FORCEINLINE float GetYawSpeed() const { return YawSpeed; }

	UFUNCTION(BlueprintPure, meta=(BlueprintThreadSafe))
	FORCEINLINE float GetSmoothedYawSpeed() const { return SmoothedYawSpeed; }

	UFUNCTION(BlueprintPure, meta=(BlueprintThreadSafe))
	FORCEINLINE bool GetIsJumping() const { return bIsJumping; }

	UFUNCTION(BlueprintPure, meta=(BlueprintThreadSafe))
	FORCEINLINE bool GetIsOnGround() const { return !bIsJumping; }

	UFUNCTION(BlueprintPure, meta=(BlueprintThreadSafe))
	FORCEINLINE float GetLookYawOffset() const { return LookRotOffset.Yaw; }

	UFUNCTION(BlueprintPure, meta=(BlueprintThreadSafe))
	FORCEINLINE float GetLookPitchOffset() const { return LookRotOffset.Pitch; }

	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	bool ShouldDoFullBody() const;

private:
	UPROPERTY()
	ACharacter* OwnerCharacter;

	UPROPERTY()
	UCharacterMovementComponent* OwnerMovementComp;

	float Speed;
	/* Yaw旋转速度 */
	FRotator BodyPrevRot;
	float YawSpeed;
	float SmoothedYawSpeed;

	UPROPERTY(EditAnywhere, Category = "Animation")
	float YawSpeedSmoothLerpSpeed = 1.f;
	UPROPERTY(EditAnywhere, Category = "Animation")
	float YawSpeedLerpToZeroSpeed = 30.f;
	/********/

	/* 跳跃相关 */
	bool bIsJumping;
	/********/

	/* Look Offset */
	FRotator LookRotOffset;
	/********/
};

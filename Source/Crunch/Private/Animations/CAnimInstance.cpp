// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/CAnimInstance.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "CGameplayTags.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UCAnimInstance::NativeInitializeAnimation()
{
	OwnerCharacter = Cast<ACharacter>(TryGetPawnOwner());
	if (OwnerCharacter)
	{
		OwnerMovementComp = OwnerCharacter->GetCharacterMovement();
	}

	UAbilitySystemComponent* OwnerASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TryGetPawnOwner());
	if (OwnerASC)
	{
		OwnerASC->RegisterGameplayTagEvent(CGameplayTags::Crunch_Stats_Aim).AddUObject(
			this, &ThisClass::OwnerAimTagUpdated
		);
	}
}

void UCAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	if (OwnerCharacter)
	{
		FVector Velocity = OwnerCharacter->GetVelocity();
		Speed = Velocity.Length();

		FRotator BodyRot = OwnerCharacter->GetActorRotation();
		FRotator BodyRotDelta = UKismetMathLibrary::NormalizedDeltaRotator(BodyRot, BodyPrevRot);
		BodyPrevRot = BodyRot;
		YawSpeed = BodyRotDelta.Yaw / DeltaSeconds;
		float YawLerpSpeed = YawSpeedSmoothLerpSpeed;
		if (YawSpeed)
		{
			YawLerpSpeed = YawSpeedLerpToZeroSpeed;
		}
		SmoothedYawSpeed = UKismetMathLibrary::FInterpTo(SmoothedYawSpeed, YawSpeed, DeltaSeconds, YawLerpSpeed);

		FRotator ControlRot = OwnerCharacter->GetBaseAimRotation();
		LookRotOffset = UKismetMathLibrary::NormalizedDeltaRotator(ControlRot, BodyRot);
		
		FwdSpeed = Velocity.Dot(ControlRot.Vector()); // .Vector()会将转向变成单位向量
		RightSpeed = -Velocity.Dot(ControlRot.Vector().Cross(FVector::UpVector));
	}

	if (OwnerMovementComp)
	{
		bIsJumping = OwnerMovementComp->IsFalling();
	}
}

void UCAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
}

bool UCAnimInstance::ShouldDoUpperBodyBlend() const
{
	return (GetSpeed() > 0) || (GetIsAiming());
}

void UCAnimInstance::OwnerAimTagUpdated(const FGameplayTag Tag, int32 NewCount)
{
	bIsAiming = NewCount != 0;
}

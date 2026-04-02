// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Crunch/Public/Characters/CCharacter.h"
#include "InputMappingContext.h"
#include "CPlayerCharacter.generated.h"

struct FInputActionValue;
class UInputAction;
class USpringArmComponent;
class UCameraComponent;

UCLASS()
class CRUNCH_API ACPlayerCharacter : public ACCharacter
{
	GENERATED_BODY()

public:
	ACPlayerCharacter();

	virtual void PawnClientRestart() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	/**********************************************************************/
	/*                         Gameplay Ability                           */
	/**********************************************************************/
private:
	virtual void OnAimStateChanged(bool bIsAiming) override;

	/**********************************************************************/
	/*                              Input                                 */
	/**********************************************************************/
private:
	UPROPERTY(EditDefaultsOnly, Category="Input")
	UInputAction* JumpInputAction;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	UInputAction* LookInputAction;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	UInputAction* MoveInputAction;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TMap<ECAbilityInputID, UInputAction*> GameplayAbilityInputActions;
	
	UPROPERTY(EditDefaultsOnly, Category="Input")
	UInputMappingContext* GameplayInputMappingContext;

	void HandleLookInput(const FInputActionValue& InputActionValue);
	void HandleMoveInput(const FInputActionValue& InputActionValue);

	void HandleAbilityInput(const FInputActionValue& InputActionValue, ECAbilityInputID InputID);

	void SetInputEnabledFromPlayerController(bool bEnabled);
	
	/**********************************************************************/
	/*                         Death And Respawn                          */
	/**********************************************************************/

protected:
	virtual void OnDeath() override;
	virtual void OnRespawn() override;

	/**********************************************************************/
	/*                               Stun                                 */
	/**********************************************************************/
protected:
	virtual void OnStun() override;
	virtual void OnRecoverFromStun() override;
	
	/**********************************************************************/
	/*                           Camera View                              */
	/**********************************************************************/
private:
	UPROPERTY(VisibleDefaultsOnly, Category="View")
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleDefaultsOnly, Category="View")
	UCameraComponent* ViewCamera;

	UPROPERTY(EditDefaultsOnly, Category="View")
	FVector CameraAimLocalOffset;

	UPROPERTY(EditDefaultsOnly, Category="View")
	float CameraLerpSpeed = 20.f;
	
	FTimerHandle CameraLerpTimerHandle;

	void LerpCameraToLocalOffsetLocation(const FVector& Goal);
	void TickCameraLocalOffsetLerp(FVector Goal);
};

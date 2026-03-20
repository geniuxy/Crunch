// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Crunch/Public/Characters/CCharacter.h"
#include "InputMappingContext.h"
#include "CPlayerCharacter.generated.h"

enum class ECAbilityInputID : uint8;
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

private:
	UPROPERTY(VisibleDefaultsOnly, Category="View")
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleDefaultsOnly, Category="View")
	UCameraComponent* ViewCamera;

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
};

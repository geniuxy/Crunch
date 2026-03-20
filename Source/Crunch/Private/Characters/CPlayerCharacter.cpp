// Fill out your copyright notice in the Description page of Project Settings.


#include "Crunch/Public/Characters/CPlayerCharacter.h"

#include "AbilitySystemComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"


ACPlayerCharacter::ACPlayerCharacter()
{
	bUseControllerRotationYaw = false;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>("Camera Boom");
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->bUsePawnControlRotation = true;

	ViewCamera = CreateDefaultSubobject<UCameraComponent>("View Camera");
	ViewCamera->SetupAttachment(CameraBoom);

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 300.f, 0.f);
}

void ACPlayerCharacter::PawnClientRestart()
{
	Super::PawnClientRestart();

	APlayerController* OwningPlayerController = GetController<APlayerController>();
	if (OwningPlayerController)
	{
		UEnhancedInputLocalPlayerSubsystem* InputSubsystem =
			OwningPlayerController->GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
		if (InputSubsystem)
		{
			InputSubsystem->RemoveMappingContext(GameplayInputMappingContext);
			InputSubsystem->AddMappingContext(GameplayInputMappingContext, 0);
		}
	}
}

void ACPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (EnhancedInputComponent)
	{
		EnhancedInputComponent->BindAction(
			JumpInputAction, ETriggerEvent::Triggered, this, &ACPlayerCharacter::Jump
		);
		EnhancedInputComponent->BindAction(
			LookInputAction, ETriggerEvent::Triggered, this, &ACPlayerCharacter::HandleLookInput
		);
		EnhancedInputComponent->BindAction(
			MoveInputAction, ETriggerEvent::Triggered, this, &ACPlayerCharacter::HandleMoveInput
		);

		for (const TPair<ECAbilityInputID, UInputAction*>& InputActionPair : GameplayAbilityInputActions)
		{
			EnhancedInputComponent->BindAction(
				InputActionPair.Value, ETriggerEvent::Triggered, this,
				&ACPlayerCharacter::HandleAbilityInput, InputActionPair.Key
			);
		}
	}
}

void ACPlayerCharacter::HandleLookInput(const FInputActionValue& InputActionValue)
{
	FVector2D LookInputVector = InputActionValue.Get<FVector2D>();

	if (LookInputVector.Y != 0.f)
	{
		AddControllerPitchInput(LookInputVector.Y);
	}
	if (LookInputVector.X != 0.f)
	{
		AddControllerYawInput(LookInputVector.X);
	}
}

void ACPlayerCharacter::HandleMoveInput(const FInputActionValue& InputActionValue)
{
	FVector2D MoveInputVector = InputActionValue.Get<FVector2D>();

	const FRotator ControllerRotation(0.f, GetControlRotation().Yaw, 0.f);

	if (MoveInputVector.Y != 0.f)
	{
		FVector TargetForwardDirection = ControllerRotation.RotateVector(FVector::ForwardVector);
		AddMovementInput(TargetForwardDirection, MoveInputVector.Y);
	}

	if (MoveInputVector.X != 0.f)
	{
		FVector TargetRightDirection = ControllerRotation.RotateVector(FVector::RightVector);
		AddMovementInput(TargetRightDirection, MoveInputVector.X);
	}
}

void ACPlayerCharacter::HandleAbilityInput(const FInputActionValue& InputActionValue, ECAbilityInputID InputID)
{
	bool bPressed = InputActionValue.Get<bool>();
	if (bPressed)
	{
		GetAbilitySystemComponent()->AbilityLocalInputPressed((int32)InputID);
	}
	else
	{
		GetAbilitySystemComponent()->AbilityLocalInputReleased((int32)InputID);
	}
}

void ACPlayerCharacter::OnDeath()
{
	APlayerController* PlayerController = GetController<APlayerController>();
	if (PlayerController)
	{
		DisableInput(PlayerController);
	}
}

void ACPlayerCharacter::OnRespawn()
{
	APlayerController* PlayerController = GetController<APlayerController>();
	if (PlayerController)
	{
		EnableInput(PlayerController);
	}
}

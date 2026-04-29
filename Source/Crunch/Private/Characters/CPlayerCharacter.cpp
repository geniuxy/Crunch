// Fill out your copyright notice in the Description page of Project Settings.


#include "Crunch/Public/Characters/CPlayerCharacter.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "CGameplayTags.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GAS/CHeroAttributeSet.h"
#include "Inventory/InventoryComponent.h"


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

	HeroAttributeSet = CreateDefaultSubobject<UCHeroAttributeSet>("HeroAttributeSet");

	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>("InventoryComponent");
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
		EnhancedInputComponent->BindAction(
			LearnAbilityLeaderAction, ETriggerEvent::Started, this, &ACPlayerCharacter::LearnAbilityLeaderPressedDown
		);
		EnhancedInputComponent->BindAction(
			LearnAbilityLeaderAction, ETriggerEvent::Completed, this, &ACPlayerCharacter::LearnAbilityLeaderPressedUp
		);

		for (const TPair<ECAbilityInputID, UInputAction*>& InputActionPair : GameplayAbilityInputActions)
		{
			EnhancedInputComponent->BindAction(
				InputActionPair.Value, ETriggerEvent::Triggered, this,
				&ACPlayerCharacter::HandleAbilityInput, InputActionPair.Key
			);
		}

		EnhancedInputComponent->BindAction(
			UseInventoryItemAction, ETriggerEvent::Triggered, this, &ACPlayerCharacter::HandleUseInventoryItem
		);
	}
}

void ACPlayerCharacter::OnAimStateChanged(bool bIsAiming)
{
	if (IsLocallyControlledByPlayer())
	{
		LerpCameraToLocalOffsetLocation(bIsAiming ? CameraAimLocalOffset : FVector(0.f));
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

void ACPlayerCharacter::LearnAbilityLeaderPressedDown(const FInputActionValue& InputActionValue)
{
	bIsLearnAbilityLeaderPressedDown = true;
}

void ACPlayerCharacter::LearnAbilityLeaderPressedUp(const FInputActionValue& InputActionValue)
{
	bIsLearnAbilityLeaderPressedDown = false;
}

void ACPlayerCharacter::HandleAbilityInput(const FInputActionValue& InputActionValue, ECAbilityInputID InputID)
{
	bool bPressed = InputActionValue.Get<bool>();

	if (bPressed && bIsLearnAbilityLeaderPressedDown)
	{
		UpgradeAbilityWithInputID(InputID);
		return;
	}

	if (bPressed)
	{
		GetAbilitySystemComponent()->AbilityLocalInputPressed((int32)InputID);
	}
	else
	{
		GetAbilitySystemComponent()->AbilityLocalInputReleased((int32)InputID);
	}

	if (InputID == ECAbilityInputID::BasicAttack)
	{
		FGameplayTag BasicAttackTag;
		if (bPressed)
		{
			BasicAttackTag = CGameplayTags::Crunch_Ability_BasicAttack_Event_Pressed;
		}
		else
		{
			BasicAttackTag = CGameplayTags::Crunch_Ability_BasicAttack_Event_Released;
		}
		// 这里Client和Server都发送GameplayEvent，我猜是因为需要做到本地预测
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
			this, BasicAttackTag, FGameplayEventData()
		);
		Server_SendGameplayEventToSelf(BasicAttackTag, FGameplayEventData());
	}
}

void ACPlayerCharacter::HandleUseInventoryItem(const FInputActionValue& InputActionValue)
{
	int Value = FMath::RoundToInt(InputActionValue.Get<float>());
	InventoryComponent->TryActivateItemInSlot(Value - 1);
}

void ACPlayerCharacter::SetInputEnabledFromPlayerController(bool bEnabled)
{
	APlayerController* PlayerController = GetController<APlayerController>();
	if (!PlayerController) return;

	if (bEnabled)
	{
		EnableInput(PlayerController);
	}
	else
	{
		DisableInput(PlayerController);
	}
}

void ACPlayerCharacter::OnDeath()
{
	SetInputEnabledFromPlayerController(false);
}

void ACPlayerCharacter::OnRespawn()
{
	SetInputEnabledFromPlayerController(true);
}

void ACPlayerCharacter::OnStun()
{
	SetInputEnabledFromPlayerController(false);
}

void ACPlayerCharacter::OnRecoverFromStun()
{
	SetInputEnabledFromPlayerController(true);
}

void ACPlayerCharacter::LerpCameraToLocalOffsetLocation(const FVector& Goal)
{
	PendingCameraGoal = Goal;

	// 后续可以加个Tag，在1秒时间内不让玩家连续多次瞄准(已完成)
	// 这是为了防止连续取消，Set多个Timer导致镜头抖动
	// 如果已在插值中，只更新目标，不重启 Timer
	if (bIsCameraLerping)
	{
		return;
	}

	bIsCameraLerping = true;
	GetWorldTimerManager().ClearTimer(CameraLerpTimerHandle);
	CameraLerpTimerHandle = GetWorldTimerManager().SetTimerForNextTick(
		FTimerDelegate::CreateUObject(this, &ThisClass::TickCameraLocalOffsetLerp)
	);
}

void ACPlayerCharacter::TickCameraLocalOffsetLerp()
{
	FVector CurrentLocalOffset = ViewCamera->GetRelativeLocation();
	if (FVector::Dist(CurrentLocalOffset, PendingCameraGoal) < 1.f)
	{
		ViewCamera->SetRelativeLocation(PendingCameraGoal);
		bIsCameraLerping = false; // 清除状态
		return;
	}

	float LerpAlpha = FMath::Clamp(GetWorld()->GetDeltaSeconds() * CameraLerpSpeed, 0.f, 1.f);
	FVector NewLocalOffset = FMath::Lerp(CurrentLocalOffset, PendingCameraGoal, LerpAlpha);
	ViewCamera->SetRelativeLocation(NewLocalOffset);

	CameraLerpTimerHandle = GetWorldTimerManager().SetTimerForNextTick(
		FTimerDelegate::CreateUObject(this, &ThisClass::TickCameraLocalOffsetLerp)
	);
}

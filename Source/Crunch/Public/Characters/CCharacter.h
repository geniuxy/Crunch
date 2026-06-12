// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "AbilitySystemInterface.h"
#include "CTypes/CEnum.h"
#include "GameFramework/Character.h"
#include "GameplayTagContainer.h"
#include "GenericTeamAgentInterface.h"
#include "Widgets/Render/RenderActorTargetInterface.h"
#include "CCharacter.generated.h"

class UGameplayAbility;
class UCAttributeSet;
class UCAbilitySystemComponent;
class UWidgetComponent;

UCLASS()
class CRUNCH_API ACCharacter : public ACharacter, public IAbilitySystemInterface, public IGenericTeamAgentInterface,
                               public IRenderActorTargetInterface
{
	GENERATED_BODY()

public:
	ACCharacter();

	void ServerSideInit();
	void ClientSideInit();

	bool IsLocallyControlledByPlayer();

protected:
	virtual void BeginPlay() override;

	virtual void PossessedBy(AController* NewController) override;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	/**********************************************************************/
	/*                         Gameplay Ability                           */
	/**********************************************************************/
public:
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SendGameplayEventToSelf(FGameplayTag EventTag, FGameplayEventData Payload);

	const TMap<ECAbilityInputID, TSubclassOf<UGameplayAbility>>& GetAbilities() const;

protected:
	void UpgradeAbilityWithInputID(ECAbilityInputID InputID);

private:
	void BindGASChangeDelegates();

	void DeathTagUpdated(FGameplayTag Tag, int NewCount);
	void StunTagUpdated(FGameplayTag Tag, int NewCount);

	void AimTagUpdated(FGameplayTag Tag, int NewCount);
	void SetIsAiming(bool bIsAiming);
	virtual void OnAimStateChanged(bool bIsAiming);

	void FocusTagUpdated(FGameplayTag Tag, int NewCount);
	bool bIsInFocusMode = false;

	void MoveSpeedUpdated(const FOnAttributeChangeData& Data);
	void MoveSpeedAccelerationUpdated(const FOnAttributeChangeData& Data);
	void MaxHealthUpdated(const FOnAttributeChangeData& Data);
	void MaxManaUpdated(const FOnAttributeChangeData& Data);

	UPROPERTY(VisibleDefaultsOnly, Category = "Gameplay Ability")
	UCAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY()
	UCAttributeSet* AttributeSet;

public:
	FORCEINLINE bool GetIsInFocusMode() const { return bIsInFocusMode; }
	/**********************************************************************/
	/*                                UI                                  */
	/**********************************************************************/
public:
	void ConfigureOverHeadStatsWidget();

private:
	UPROPERTY(VisibleDefaultsOnly, Category="UI")
	UWidgetComponent* OverHeadWidgetComponent;

	UPROPERTY(EditDefaultsOnly, Category="UI")
	float HeadStatsGaugeVisibilityCheckUpdateGap = 1.f;

	UPROPERTY(EditDefaultsOnly, Category="UI")
	float HeadStatsGaugeVisibilityRangeSquared = 1000000.f; // 头顶血条可视距离的平方

	FTimerHandle HeadStatsGaugeVisibilityUpdateTimerHandle;

	void UpdateHeadGaugeVisibility();

	void SetStatusGaugeEnabled(bool bIsEnabled);

	/**********************************************************************/
	/*                        Death And Respawn                           */
	/**********************************************************************/
protected:
	bool IsDead() const;

	void RespawnImmediately();

private:
	FTransform MeshRelativeTransform; // 记录Mesh初始RelativeTransform，用于Ragdoll后恢复位置

	UPROPERTY(EditDefaultsOnly, Category="Death")
	float DeathMontageFinishTimeShift = -0.8f;

	UPROPERTY(EditDefaultsOnly, Category="Death")
	UAnimMontage* DeathMontage;

	FTimerHandle DeathMontageTimerHandle;

	void DeathMontageFinished();
	void SetRagDollEnabled(bool bIsEnabled);

	void PlayDeathAnimation();

	void StartDeathSequence();
	void Respawn();

protected:
	virtual void OnDeath();
	virtual void OnRespawn();

	/**********************************************************************/
	/*                               Stun                                 */
	/**********************************************************************/

private:
	UPROPERTY(EditDefaultsOnly, Category="Stun")
	UAnimMontage* StunMontage;

protected:
	virtual void OnStun();
	virtual void OnRecoverFromStun();

	/**********************************************************************/
	/*                                Hit                                 */
	/**********************************************************************/
protected:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Hit")
	UAnimMontage* HitMontage;

	/**********************************************************************/
	/*                                Team                                */
	/**********************************************************************/
public:
	//~ Begin IGenericTeamAgentInterface Interface
	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;
	virtual FGenericTeamId GetGenericTeamId() const override;
	//~ End IGenericTeamAgentInterface Interface
protected:
	UPROPERTY(ReplicatedUsing=OnRep_TeamID)
	FGenericTeamId TeamID;

	UFUNCTION()
	virtual void OnRep_TeamID();
	/**********************************************************************/
	/*                                AI                                  */
	/**********************************************************************/
private:
	void SetAIPerceptionStimuliSourceEnabled(bool bIsEnabled);

	// 赋予拥有该角色的演员一种自动注册为感知系统感觉刺激源
	UPROPERTY()
	class UAIPerceptionStimuliSourceComponent* PerceptionStimuliSourceComponent;

	/**********************************************************************/
	/*                           Render Actor                             */
	/**********************************************************************/
public:
	virtual FVector GetCaptureLocalPosition() const override;
	virtual FRotator GetCaptureLocalRotation() const override;

private:
	UPROPERTY(EditDefaultsOnly, Category="Capture")
	FVector HeadshotCaptureLocalPosition;

	UPROPERTY(EditDefaultsOnly, Category="Capture")
	FRotator HeadshotCaptureLocalRotation;

	/**********************************************************************/
	/*                               Net                                  */
	/**********************************************************************/
protected:
	UPROPERTY(EditDefaultsOnly, Category="Net")
	bool bDebugNetworkInfo = false;

	void GetNetworkDebugInfo() const;

	void TestPlayerPawn();
};

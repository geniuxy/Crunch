// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "Characters/CPlayerCharacter.h"
#include "CAbilitySystemComponent.generated.h"

class UPA_AbilitySystemGenerics;

DECLARE_MULTICAST_DELEGATE_ThreeParams(
	FOnCooldownTimeUpdated,
	UGameplayAbility* /*Handle*/, float /*NewRemaining*/, float /*NewDuration*/
)
/**
 * 
 */
UCLASS()
class CRUNCH_API UCAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	UCAbilitySystemComponent();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	void ServerSideInit();

	void ApplyFullStatsEffect();

	void DisableAim();

	bool IsAtMaxLevel() const;

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_UpgradeAbilityWithID(ECAbilityInputID InputID);

	UFUNCTION(Client, Reliable)
	void Client_AbilitySpecLevelUpdated(FGameplayAbilitySpecHandle Handle, int NewLevel);

private:
	void InitializeBaseAttributes();

	void InitializeBaseGameplayEffects();

	void GiveInitialAbilities();

	void AuthApplyGameplayEffect(TSubclassOf<UGameplayEffect> GameplayEffect, int Level = 1);

	void HealthUpdated(const FOnAttributeChangeData& ChangeData);
	void ManaUpdated(const FOnAttributeChangeData& ChangeData);
	void ExperienceUpdated(const FOnAttributeChangeData& ChangeData);
	void CooldownReductionUpdated(const FOnAttributeChangeData& ChangeData);

	UPROPERTY(EditDefaultsOnly, Category="Gameplay Ability")
	TMap<ECAbilityInputID, TSubclassOf<UGameplayAbility>> Abilities; // 可学习的Abilities（初始0级）

	UPROPERTY(EditDefaultsOnly, Category="Gameplay Ability")
	TMap<ECAbilityInputID, TSubclassOf<UGameplayAbility>> BasicAbilities; // 初始Abilities（初始1级）

	UPROPERTY(EditDefaultsOnly, Category="Gameplay Ability")
	UPA_AbilitySystemGenerics* AbilitySystemGenerics;

public:
	FORCEINLINE const TMap<ECAbilityInputID, TSubclassOf<UGameplayAbility>>& GetAbilities() const { return Abilities; }

	/**********************************************************************/
	/*                            Aim Target                              */
	/**********************************************************************/
private:
	UPROPERTY(Replicated)
	TObjectPtr<AActor> AimTarget;

	UFUNCTION(Server, Reliable)
	void Server_SetAimTarget(AActor* NewTarget);

public:
	void ClearAimTarget();

	FORCEINLINE AActor* GetAimTarget() const { return AimTarget; }
	void SetAimTarget(AActor* NewAimTarget);

	/**********************************************************************/
	/*                       Modify Cooldown Time                         */
	/**********************************************************************/
public:
	FOnCooldownTimeUpdated OnCooldownTimeUpdated;

private:
	// 修改所有冷却 GE 的剩余时间
	void ModifyAllCooldownEffectsRemainingTime(float NewReduction, float OldReduction);

	// 修改单个 GE 的剩余时间
	void ModifyActiveEffectRemainingTime(FActiveGameplayEffectHandle Handle, float NewReduction, float OldReduction);

	UFUNCTION(Client, Reliable)
	void Client_OnCooldownTimeUpdated(UGameplayAbility* Ability, float NewRemaining, float NewDuration);
};

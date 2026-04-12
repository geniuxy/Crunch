// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "Characters/CPlayerCharacter.h"
#include "CAbilitySystemComponent.generated.h"

class UPA_AbilitySystemGenerics;
/**
 * 
 */
UCLASS()
class CRUNCH_API UCAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	UCAbilitySystemComponent();

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

	UPROPERTY(EditDefaultsOnly, Category="Gameplay Ability")
	TMap<ECAbilityInputID, TSubclassOf<UGameplayAbility>> Abilities; // 指代AvatarActor独有的Abilities

	UPROPERTY(EditDefaultsOnly, Category="Gameplay Ability")
	TMap<ECAbilityInputID, TSubclassOf<UGameplayAbility>> BasicAbilities; // 通用的Abilities

	UPROPERTY(EditDefaultsOnly, Category="Gameplay Ability")
	UPA_AbilitySystemGenerics* AbilitySystemGenerics;

public:
	FORCEINLINE const TMap<ECAbilityInputID, TSubclassOf<UGameplayAbility>>& GetAbilities() const { return Abilities; }
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "Characters/CPlayerCharacter.h"
#include "CAbilitySystemComponent.generated.h"

/**
 * 
 */
UCLASS()
class CRUNCH_API UCAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	UCAbilitySystemComponent();

	void InitializeBaseAttributes();

	void GiveInitialAbilities();

	void ApplyFullStatsEffect();

	void DisableAim();
	
private:
	void AuthApplyGameplayEffect(TSubclassOf<UGameplayEffect> GameplayEffect, int Level = 1);
	
	void HealthUpdated(const FOnAttributeChangeData& ChangeData);

	UPROPERTY(EditDefaultsOnly, Category="Gameplay Effects")
	TSubclassOf<UGameplayEffect> FullStatsEffect;

	UPROPERTY(EditDefaultsOnly, Category="Gameplay Effects")
	TSubclassOf<UGameplayEffect> DeathEffect;

	UPROPERTY(EditDefaultsOnly, Category="Gameplay Effects")
	TSubclassOf<UGameplayEffect> DisableAimEffect;

	UPROPERTY(EditDefaultsOnly, Category="Gameplay Ability")
	TArray<TSubclassOf<UGameplayEffect>> InitializeGameplayEffects;

	UPROPERTY(EditDefaultsOnly, Category="Gameplay Ability")
	TMap<ECAbilityInputID, TSubclassOf<UGameplayAbility>> Abilities; // 指代AvatarActor独有的Abilities

	UPROPERTY(EditDefaultsOnly, Category="Gameplay Ability")
	TMap<ECAbilityInputID, TSubclassOf<UGameplayAbility>> BasicAbilities; // 通用的Abilities

public:
	FORCEINLINE const TMap<ECAbilityInputID, TSubclassOf<UGameplayAbility>>& GetAbilities() const { return Abilities; }
};

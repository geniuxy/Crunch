// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilitySpec.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Blueprint/UserWidget.h"
#include "AbilityGauge.generated.h"

struct FGameplayAbilitySpec;
class UAbilitySystemComponent;
class UGameplayAbility;
struct FAbilityWidgetData;
class UTextBlock;
class UImage;
/**
 * 
 */
UCLASS()
class CRUNCH_API UAbilityGauge : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

	void ConfigureWithWidgetData(const FAbilityWidgetData* WidgetData);

private:
	UPROPERTY(meta=(BindWidget))
	UImage* Icon;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* CooldownCounterText; // Counter计数器，这里指冷却倒计时

	UPROPERTY(meta=(BindWidget))
	UTextBlock* CooldownDurationText;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* CostText;

	UPROPERTY(meta=(BindWidget))
	UImage* AbilityLevelGauge;

	UPROPERTY()
	UGameplayAbility* AbilityCDO; // AbilityGauge所指向的Ability, CDO(Class Default Object)

	void AbilityCommit(UGameplayAbility* Ability);

	void StartCooldown(float CooldownTimeRemaining, float CooldownDuration);

	void CooldownFinished();
	void UpdateCooldown();

	float CachedCooldownDuration;
	float CachedCooldownTimeRemaining;

	FTimerHandle CooldownTimerHandle;
	FTimerHandle CooldownTimerUpdateHandle;

	FNumberFormattingOptions WholeNumberFormattingOptions;
	FNumberFormattingOptions OneDigitNumberFormattingOptions;

	UPROPERTY()
	UAbilitySystemComponent* OwnerAbilitySystemComponent;

	const FGameplayAbilitySpec* CachedAbilitySpec;

	const FGameplayAbilitySpec* GetAbilitySpec();

	bool bIsAbilityLearned = false;

	void AbilitySpecUpdated(const FGameplayAbilitySpec& AbilitySpec);
	void UpdateCanCast();
	void UpgradePointUpdated(const FOnAttributeChangeData& Data);
	void ManaUpdated(const FOnAttributeChangeData& Data);
	
	UPROPERTY(EditDefaultsOnly, Category="Cooldown")
	float CooldownUpdateInterval = 0.1f;
	
	UPROPERTY(EditDefaultsOnly, Category="Visual")
	FName IconMaterialParamName = "Icon";

	UPROPERTY(EditDefaultsOnly, Category="Visual")
	FName CooldownPercentParamName = "Percent";

	UPROPERTY(EditDefaultsOnly, Category="Visual")
	FName AbilityLevelParamName = "Level";

	UPROPERTY(EditDefaultsOnly, Category="Visual")
	FName CanCastAbilityParamName = "CanCast";

	UPROPERTY(EditDefaultsOnly, Category="Visual")
	FName UpgradePointAvaliableParamName = "UpgradeAvaliable";
};

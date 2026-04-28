// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayWidget.generated.h"

class UMatchStatWidget;
class USkeletalMeshRenderWidget;
class UInventoryWidget;
class UShopWidget;
class UStatsGauge;
class UAbilityListView;
class UGameplayAbility;
enum class ECAbilityInputID : uint8;
class UAbilitySystemComponent;
class UValueGauge;
/**
 * 
 */
UCLASS()
class CRUNCH_API UGameplayWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	void ConfigureAbilities(const TMap<ECAbilityInputID, TSubclassOf<UGameplayAbility>>& Abilities);

	void ToggleShop();

private:
	UPROPERTY(meta=(BindWidget))
	UValueGauge* HealthBar;

	UPROPERTY(meta=(BindWidget))
	UValueGauge* ManaBar;

	UPROPERTY(meta=(BindWidget))
	UAbilityListView* AbilityListView;

	UPROPERTY(meta=(BindWidget))
	UStatsGauge* AttackDamageGauge;

	UPROPERTY(meta=(BindWidget))
	UStatsGauge* ArmorGauge;

	UPROPERTY(meta=(BindWidget))
	UStatsGauge* IntelligenceGauge;

	UPROPERTY(meta=(BindWidget))
	UStatsGauge* StrengthGauge;

	UPROPERTY(meta=(BindWidget))
	UStatsGauge* MoveSpeedGauge;

	UPROPERTY(meta=(BindWidget))
	UShopWidget* ShopWidget;

	UPROPERTY(meta=(BindWidget))
	UInventoryWidget* InventoryWidget;

	UPROPERTY(meta=(BindWidget))
	USkeletalMeshRenderWidget* HeadshotWidget;

	UPROPERTY(meta=(BindWidget))
	UMatchStatWidget* MatchStatWidget;
	
	UPROPERTY(Transient, meta=(BindWidgetAnim))
	UWidgetAnimation* ShopPopUpAnimation;

	void PlayShopPopUpAnimation(bool bPlayForward);
	void SetOwningPawnInputEnabled(bool bPawnInputEnabled);
	void SetShowMouseCursor(bool bShowMouseCursor);
	void SetFocusToGameAndUI();
	void SetFocusToGameOnly();

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> OwnerAbilitySystemComponent;
};

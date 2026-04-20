// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PA_ShopItem.generated.h"

class UGameplayAbility;
class UGameplayEffect;
/**
 * 
 */
UCLASS()
class CRUNCH_API UPA_ShopItem : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;
	static FPrimaryAssetType GetShopItemAssetType();

	UTexture2D* GetIcon() const;

private:
	UPROPERTY(EditDefaultsOnly, Category="ShopItem")
	TSoftObjectPtr<UTexture2D> Icon;

	UPROPERTY(EditDefaultsOnly, Category="ShopItem")
	float Price;

	UPROPERTY(EditDefaultsOnly, Category="ShopItem")
	FText ItemName;

	UPROPERTY(EditDefaultsOnly, Category="ShopItem")
	FText ItemDescription;

	UPROPERTY(EditDefaultsOnly, Category="ShopItem")
	bool bIsConsumable;

	UPROPERTY(EditDefaultsOnly, Category="ShopItem")
	TSubclassOf<UGameplayEffect> EquippedEffect;

	UPROPERTY(EditDefaultsOnly, Category="ShopItem")
	TSubclassOf<UGameplayEffect> ConsumeEffect;

	UPROPERTY(EditDefaultsOnly, Category="ShopItem")
	TSubclassOf<UGameplayAbility> GrantedAbility;

	UPROPERTY(EditDefaultsOnly, Category="ShopItem")
	bool bIsStackable = false;

	UPROPERTY(EditDefaultsOnly, Category="ShopItem")
	int MaxStackCount = 5;

	UPROPERTY(EditDefaultsOnly, Category="ShopItem")
	TArray<TSoftObjectPtr<UPA_ShopItem>> IngredientItems;

public:
	// 内联作用： 建议编译器将函数调用替换为函数体代码，避免函数调用的开销。
	FORCEINLINE float GetPrice() const { return Price; }
	FORCEINLINE float GetSellPrice() const { return Price / 2.f; }
	FORCEINLINE FText GetItemName() const { return ItemName; }
	FORCEINLINE FText GetItemDescription() const { return ItemDescription; }
	FORCEINLINE bool GetIsConsumable() const { return bIsConsumable; }
	FORCEINLINE TSubclassOf<UGameplayEffect> GetEquippedEffect() const { return EquippedEffect; }
	FORCEINLINE TSubclassOf<UGameplayEffect> GetConsumeEffect() const { return ConsumeEffect; }
	FORCEINLINE TSubclassOf<UGameplayAbility> GetGrantedAbility() const { return GrantedAbility; }
	FORCEINLINE bool GetIsStackable() const { return bIsStackable; }
	FORCEINLINE int GetMaxStackCount() const { return MaxStackCount; }
	FORCEINLINE const TArray<TSoftObjectPtr<UPA_ShopItem>>& GetIngredients() const { return IngredientItems; }
};

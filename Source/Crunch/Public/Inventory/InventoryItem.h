// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "GameplayAbilitySpecHandle.h"
#include "Data/PA_ShopItem.h"
#include "UObject/Object.h"
#include "InventoryItem.generated.h"

struct FGameplayAbilitySpecHandle;
class UAbilitySystemComponent;
class UPA_ShopItem;

USTRUCT()
struct FInventoryItemHandle
{
	GENERATED_BODY()

public:
	FInventoryItemHandle() : HandleID(GetInvalidID())
	{
	}

	static FInventoryItemHandle InvalidHandle();
	static FInventoryItemHandle CreateHandle();

	bool IsValid() const;
	uint32 GetHandleID() const { return HandleID; }

private:
	// explicit 表示 类型不能隐式转换 uint32不能自动转为FInventoryItemHandle
	// 这个 Handle 的设计意图很明确：
	// 不允许外部直接用原始 ID 构造，必须通过 CreateHandle() 或 InvalidHandle() 工厂方法。
	explicit FInventoryItemHandle(uint32 ID) : HandleID(ID)
	{
	}

	UPROPERTY()
	uint32 HandleID;

	static uint32 GenerateNextID();
	static uint32 GetInvalidID();
};

bool operator==(const FInventoryItemHandle& Lhs, const FInventoryItemHandle& Rhs);
uint32 GetTypeHash(const FInventoryItemHandle& Key);

/**
 * 
 */
UCLASS()
class CRUNCH_API UInventoryItem : public UObject
{
	GENERATED_BODY()

public:
	void InitItem(const FInventoryItemHandle& NewHandle, const UPA_ShopItem* NewShopItem);

	bool TryActivateGrantedAbility(UAbilitySystemComponent* AbilitySystemComponent);
	void ApplyConsumeEffect(UAbilitySystemComponent* AbilitySystemComponent);
	void ApplyGASModifications(UAbilitySystemComponent* AbilitySystemComponent);
	void RemoveGASModifications(UAbilitySystemComponent* AbilitySystemComponent);

	bool IsValid() const;

	bool AddStackCount(); // 如果可以添加的话，返回true
	bool ReduceStackCount(); // 如果减少1个之后不是空的话，返回true
	bool SetStackCount(int NewStackCount); // 如果可以设置的话，返回true
	bool IsStackFull() const;
	bool IsForItem(const UPA_ShopItem* Item) const;

private:
	UPROPERTY()
	const UPA_ShopItem* ShopItem;

	FInventoryItemHandle Handle;

	int StackCount = 1;
	int Slot;

	FActiveGameplayEffectHandle AppliedEquippedEffectHandle;
	FGameplayAbilitySpecHandle GrantedAbilitySpecHandle;

public:
	FORCEINLINE const UPA_ShopItem* GetShopItem() const { return ShopItem; }
	FORCEINLINE FInventoryItemHandle GetHandle() const { return Handle; }
	FORCEINLINE int GetStackCount() const { return StackCount; }
	void SetSlot(int NewSlot) { Slot = NewSlot;}
};

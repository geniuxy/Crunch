// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/InventoryItem.h"

#include "AbilitySystemComponent.h"
#include "Inventory/Data/PA_ShopItem.h"

FInventoryItemHandle FInventoryItemHandle::InvalidHandle()
{
	// 返回静态单例引用
	// 始终返回同一个对象"同一个静态实例"的引用
	static FInventoryItemHandle InvalidHandle = FInventoryItemHandle();
	return InvalidHandle;
}

FInventoryItemHandle FInventoryItemHandle::CreateHandle()
{
	return FInventoryItemHandle(GenerateNextID());
}

bool FInventoryItemHandle::IsValid() const
{
	return HandleID != GetInvalidID();
}

uint32 FInventoryItemHandle::GenerateNextID()
{
	static uint32 StaticID = 1;
	return StaticID++;
}

uint32 FInventoryItemHandle::GetInvalidID()
{
	return 0;
}

bool operator==(const FInventoryItemHandle& Lhs, const FInventoryItemHandle& Rhs)
{
	return Lhs.GetHandleID() == Rhs.GetHandleID();
}

uint32 GetTypeHash(const FInventoryItemHandle& Key)
{
	return Key.GetHandleID();
}

void UInventoryItem::InitItem(const FInventoryItemHandle& NewHandle, const UPA_ShopItem* NewShopItem)
{
	Handle = NewHandle;
	ShopItem = NewShopItem;
}

void UInventoryItem::ApplyGASModifications(UAbilitySystemComponent* AbilitySystemComponent)
{
	if (!GetShopItem() || !AbilitySystemComponent) return;
	if (!AbilitySystemComponent->GetOwner() || !AbilitySystemComponent->GetOwner()->HasAuthority()) return;

	if (TSubclassOf<UGameplayEffect> EquippedEffect = GetShopItem()->GetEquippedEffect())
	{
		AppliedEquippedEffectHandle = AbilitySystemComponent->BP_ApplyGameplayEffectToSelf(
			EquippedEffect, 1, AbilitySystemComponent->MakeEffectContext()
		);
	}

	if (TSubclassOf<UGameplayAbility> GrantedAbility = GetShopItem()->GetGrantedAbility())
	{
		FGameplayAbilitySpec* FoundSpec = AbilitySystemComponent->FindAbilitySpecFromClass(GrantedAbility);
		if (FoundSpec)
		{
			GrantedAbilitySpecHandle = FoundSpec->Handle;
		}
		else
		{
			GrantedAbilitySpecHandle = AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(GrantedAbility));
		}
	}
}

bool UInventoryItem::IsValid() const
{
	return ShopItem != nullptr;
}

bool UInventoryItem::AddStackCount()
{
	if (IsStackFull()) return false;

	++StackCount;
	return true;
}

bool UInventoryItem::ReduceStackCount()
{
	--StackCount;
	if (StackCount <= 0)
	{
		return false;
	}
	return true;
}

bool UInventoryItem::SetStackCount(int NewStackCount)
{
	if (NewStackCount > 0 && NewStackCount <= GetShopItem()->GetMaxStackCount())
	{
		StackCount = NewStackCount;
		return true;
	}
	return false;
}

bool UInventoryItem::IsStackFull() const
{
	return StackCount >= GetShopItem()->GetMaxStackCount();
}

bool UInventoryItem::IsForItem(const UPA_ShopItem* Item) const
{
	if (!Item) return false;

	return GetShopItem() == Item;
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/InventoryItem.h"

#include "AbilitySystemComponent.h"
#include "FunctionLibrary/CAbilitySystemFunctionLibrary.h"
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

void UInventoryItem::InitItem(
	const FInventoryItemHandle& NewHandle,
	const UPA_ShopItem* NewShopItem,
	UAbilitySystemComponent* AbilitySystemComponent)
{
	Handle = NewHandle;
	ShopItem = NewShopItem;

	OwnerAbilitySystemComponent = AbilitySystemComponent;
	ApplyGASModifications();
}

bool UInventoryItem::TryActivateGrantedAbility()
{
	if (!GrantedAbilitySpecHandle.IsValid()) return false;

	if (OwnerAbilitySystemComponent && OwnerAbilitySystemComponent->TryActivateAbility(GrantedAbilitySpecHandle))
	{
		return true;
	}

	return false;
}

void UInventoryItem::ApplyConsumeEffect()
{
	if (!ShopItem) return;

	TSubclassOf<UGameplayEffect> ConsumeEffect = ShopItem->GetConsumeEffect();
	if (!ConsumeEffect) return;

	OwnerAbilitySystemComponent->BP_ApplyGameplayEffectToSelf(
		ConsumeEffect, 1, OwnerAbilitySystemComponent->MakeEffectContext()
	);
}

void UInventoryItem::RemoveGASModifications()
{
	if (!OwnerAbilitySystemComponent) return;

	if (AppliedEquippedEffectHandle.IsValid())
	{
		OwnerAbilitySystemComponent->RemoveActiveGameplayEffect(AppliedEquippedEffectHandle);
	}

	if (GrantedAbilitySpecHandle.IsValid())
	{
		OwnerAbilitySystemComponent->SetRemoveAbilityOnEnd(GrantedAbilitySpecHandle);
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

bool UInventoryItem::IsGrantingAbility(TSubclassOf<UGameplayAbility> AbilityClass) const
{
	if (!ShopItem) return false;

	TSubclassOf<UGameplayAbility> GrantedAbility = ShopItem->GetGrantedAbility();
	return GrantedAbility == AbilityClass;
}

bool UInventoryItem::IsGrantingAnyAbility() const
{
	if (!ShopItem) return false;

	return ShopItem->GetGrantedAbility() != nullptr;
}

float UInventoryItem::GetAbilityCooldownTimerRemaining() const
{
	if (!IsGrantingAnyAbility()) return 0.f;

	return UCAbilitySystemFunctionLibrary::GetCooldownRemainingFor(
		ShopItem->GetGrantedAbilityCDO(), *OwnerAbilitySystemComponent
	);
}

float UInventoryItem::GetAbilityCooldownDuration() const
{
	if (!IsGrantingAnyAbility()) return 0.f;

	return UCAbilitySystemFunctionLibrary::GetCooldownDurationFor(
		ShopItem->GetGrantedAbilityCDO(), *OwnerAbilitySystemComponent, 1
	);
}

float UInventoryItem::GetAbilityManaCost() const
{
	if (!IsGrantingAnyAbility()) return 0.f;

	return UCAbilitySystemFunctionLibrary::GetManaCostFor(
		ShopItem->GetGrantedAbilityCDO(), *OwnerAbilitySystemComponent, 1
	);
}

void UInventoryItem::ApplyGASModifications()
{
	if (!GetShopItem() || !OwnerAbilitySystemComponent) return;
	if (!OwnerAbilitySystemComponent->GetOwner() || !OwnerAbilitySystemComponent->GetOwner()->HasAuthority()) return;

	if (TSubclassOf<UGameplayEffect> EquippedEffect = GetShopItem()->GetEquippedEffect())
	{
		AppliedEquippedEffectHandle = OwnerAbilitySystemComponent->BP_ApplyGameplayEffectToSelf(
			EquippedEffect, 1, OwnerAbilitySystemComponent->MakeEffectContext()
		);
	}

	if (TSubclassOf<UGameplayAbility> GrantedAbility = GetShopItem()->GetGrantedAbility())
	{
		GrantedAbilitySpecHandle = OwnerAbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(GrantedAbility));
	}
}

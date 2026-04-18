// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/InventoryComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "CrunchDebugHelper.h"
#include "GAS/CHeroAttributeSet.h"
#include "Inventory/InventoryItem.h"


UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UInventoryComponent::TryPurchase(const UPA_ShopItem* ItemToPurchase)
{
	if (!OwnerAbilitySystemComponent) return;

	Server_Purchase(ItemToPurchase);
}

float UInventoryComponent::GetGold() const
{
	if (!OwnerAbilitySystemComponent) return 0.f;

	bool bFound = false;
	float Gold = OwnerAbilitySystemComponent->GetGameplayAttributeValue(
		UCHeroAttributeSet::GetGoldAttribute(), bFound
	);

	return bFound ? Gold : 0.f;
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner());
}

void UInventoryComponent::GrantItem(const UPA_ShopItem* NewItem)
{
	if (!GetOwner()->HasAuthority()) return;

	UInventoryItem* InventoryItem = NewObject<UInventoryItem>();
	FInventoryItemHandle NewHandle = FInventoryItemHandle::CreateHandle();
	InventoryItem->InitItem(NewHandle, NewItem);
	InventoryMap.Add(NewHandle, InventoryItem);
	OnItemAdded.Broadcast(InventoryItem);
	Debug::Print(FString::Printf(
			TEXT("Server添加物品：%s, ID为：%d"),
			*InventoryItem->GetShopItem()->GetItemName().ToString(),
			NewHandle.GetHandleID())
	);
	Client_ItemAdded(NewHandle, NewItem);
	InventoryItem->ApplyGASModifications(OwnerAbilitySystemComponent);
}

void UInventoryComponent::Client_ItemAdded_Implementation(FInventoryItemHandle AssignedHandle, const UPA_ShopItem* Item)
{
	if (GetOwner()->HasAuthority()) return;

	UInventoryItem* InventoryItem = NewObject<UInventoryItem>();
	InventoryItem->InitItem(AssignedHandle, Item);
	InventoryMap.Add(AssignedHandle, InventoryItem);
	OnItemAdded.Broadcast(InventoryItem);
	Debug::Print(FString::Printf(
			TEXT("Client添加物品：%s, ID为：%d"),
			*InventoryItem->GetShopItem()->GetItemName().ToString(),
			AssignedHandle.GetHandleID())
	);
}

void UInventoryComponent::Server_Purchase_Implementation(const UPA_ShopItem* ItemToPurchase)
{
	if (!ItemToPurchase) return;
	if (GetGold() < ItemToPurchase->GetPrice()) return;

	OwnerAbilitySystemComponent->ApplyModToAttribute(
		UCHeroAttributeSet::GetGoldAttribute(), EGameplayModOp::Additive, -ItemToPurchase->GetPrice()
	);
	GrantItem(ItemToPurchase);
}

bool UInventoryComponent::Server_Purchase_Validate(const UPA_ShopItem* ItemToPurchase)
{
	return true;
}

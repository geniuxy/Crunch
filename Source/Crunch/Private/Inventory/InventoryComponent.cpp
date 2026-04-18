// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/InventoryComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "CrunchDebugHelper.h"
#include "GAS/CHeroAttributeSet.h"


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

void UInventoryComponent::Server_Purchase_Implementation(const UPA_ShopItem* ItemToPurchase)
{
	if (!ItemToPurchase) return;
	if (GetGold() < ItemToPurchase->GetPrice()) return;

	OwnerAbilitySystemComponent->ApplyModToAttribute(
		UCHeroAttributeSet::GetGoldAttribute(), EGameplayModOp::Additive, -ItemToPurchase->GetPrice()
	);
	Debug::Print(FString::Printf(TEXT("购买了物品：%s"), *ItemToPurchase->GetItemName().ToString()));
}

bool UInventoryComponent::Server_Purchase_Validate(const UPA_ShopItem* ItemToPurchase)
{
	return true;
}

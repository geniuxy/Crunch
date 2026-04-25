// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventoryItem.h"
#include "Components/ActorComponent.h"
#include "Data/PA_ShopItem.h"
#include "InventoryComponent.generated.h"

class UInventoryItem;
class UPA_ShopItem;
class UAbilitySystemComponent;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemAddedDelegate, const UInventoryItem* /* NewItem */)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemRemovedDelegate, const FInventoryItemHandle& /* ItemHandle */)
DECLARE_MULTICAST_DELEGATE_TwoParams(
	FOnItemStackCountChangedDelegate, const FInventoryItemHandle& /* ItemHandle */, int /* NewCount */)
DECLARE_MULTICAST_DELEGATE_ThreeParams(
	FOnItemAbilityCommitted, const FInventoryItemHandle& /* ItemHandle */,
	float /* CooldownDuration */, float /* CooldownTimeRemaining */
)

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CRUNCH_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInventoryComponent();

	FOnItemAddedDelegate OnItemAdded;
	FOnItemRemovedDelegate OnItemRemoved;
	FOnItemStackCountChangedDelegate OnItemStackCountChanged;
	FOnItemAbilityCommitted OnItemAbilityCommitted;

	void TryActivateItem(const FInventoryItemHandle& ItemHandle);
	void TryPurchase(const UPA_ShopItem* ItemToPurchase);
	void SellItem(const FInventoryItemHandle& ItemToPurchase);
	float GetGold() const;

	void ItemSlotChanged(const FInventoryItemHandle& Handle, int NewSlotNumber);
	UInventoryItem* GetInventoryItemByHandle(const FInventoryItemHandle& Handle) const;

	bool IsFullFor(const UPA_ShopItem* Item) const;
	bool IsAllSlotOccupied() const;
	UInventoryItem* GetAvaliableStackForItem(const UPA_ShopItem* Item) const;

	bool FindIngredientForItem(
		const UPA_ShopItem* Item,
		TArray<UInventoryItem*>& OutIngredients,
		const TArray<const UPA_ShopItem*>& IngredientToIgnore = TArray<const UPA_ShopItem*>()
	);
	UInventoryItem* TryGetItemForShopItem(const UPA_ShopItem* Item) const;

	void TryActivateItemInSlot(int SlotNumber);

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	UAbilitySystemComponent* OwnerAbilitySystemComponent;

	UPROPERTY()
	TMap<FInventoryItemHandle, UInventoryItem*> InventoryMap;

	UPROPERTY(EditDefaultsOnly, Category="Inventory")
	int Capacity = 6;

	void AbilityCommitted(UGameplayAbility* CommittedAbility);

public:
	FORCEINLINE int GetCapacity() const { return Capacity; }

	/**********************************************************************/
	/*                              Server                                */
	/**********************************************************************/
private:
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_Purchase(const UPA_ShopItem* ItemToPurchase);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_ActivateItem(FInventoryItemHandle ItemHandle);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SellItem(FInventoryItemHandle ItemHandle);

	void GrantItem(const UPA_ShopItem* NewItem);

	void ConsumeItem(UInventoryItem* Item);

	void RemoveItem(UInventoryItem* Item);

	bool TryItemCombination(const UPA_ShopItem* NewItem);

	/**********************************************************************/
	/*                              Server                                */
	/**********************************************************************/
private:
	UFUNCTION(Client, Reliable) // Client: 仅在拥有该 Actor 的本地客户端执行
	void Client_ItemAdded(
		FInventoryItemHandle AssignedHandle,
		const UPA_ShopItem* Item,
		FGameplayAbilitySpecHandle GrantedAbilitySpecHandle
	);

	UFUNCTION(Client, Reliable)
	void Client_ItemRemoved(FInventoryItemHandle ItemHandle);

	UFUNCTION(Client, Reliable)
	void Client_ItemStackCountChanged(FInventoryItemHandle Handle, int NewCount);
};

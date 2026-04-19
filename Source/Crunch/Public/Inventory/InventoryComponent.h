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

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CRUNCH_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInventoryComponent();

	FOnItemAddedDelegate OnItemAdded;

	void TryPurchase(const UPA_ShopItem* ItemToPurchase);
	float GetGold() const;

	void ItemSlotChanged(const FInventoryItemHandle& Handle, int NewSlotNumber);
	UInventoryItem* GetInventoryItemByHandle(const FInventoryItemHandle& Handle) const;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	UAbilitySystemComponent* OwnerAbilitySystemComponent;

	UPROPERTY()
	TMap<FInventoryItemHandle, UInventoryItem*> InventoryMap;

	UPROPERTY(EditDefaultsOnly, Category="Inventory")
	int Capacity = 6;

public:
	FORCEINLINE int GetCapacity() const { return Capacity; }

	/**********************************************************************/
	/*                              Server                                */
	/**********************************************************************/
private:
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_Purchase(const UPA_ShopItem* ItemToPurchase);

	void GrantItem(const UPA_ShopItem* NewItem);

	/**********************************************************************/
	/*                              Server                                */
	/**********************************************************************/
private:
	UFUNCTION(Client, Reliable) // Client: 仅在拥有该 Actor 的本地客户端执行
	void Client_ItemAdded(FInventoryItemHandle AssignedHandle, const UPA_ShopItem* Item);
};

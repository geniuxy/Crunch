// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/InventoryItem.h"

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
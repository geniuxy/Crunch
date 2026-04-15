// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Widgets/Item/ItemWidget.h"
#include "ShopItemWidget.generated.h"

class UPA_ShopItem;
/**
 * 
 */
UCLASS()
class CRUNCH_API UShopItemWidget : public UItemWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

public:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

private:
	UPROPERTY()
	const UPA_ShopItem* ShopItem;

public:
	FORCEINLINE const UPA_ShopItem* GetShopItem() const { return ShopItem; }
};

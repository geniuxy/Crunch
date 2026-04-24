// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Inventory/Data/PA_ShopItem.h"
#include "Widgets/Item/ItemWidget.h"
#include "Widgets/Tree/TreeNodeInterface.h"
#include "ShopItemWidget.generated.h"

class ITreeNodeInterface;
class UListView;
class UShopItemWidget;
class UPA_ShopItem;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnShopItemPurchaseIssued, const UPA_ShopItem*);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnShopItemSelected, const UShopItemWidget*);
/**
 * 
 */
UCLASS()
class CRUNCH_API UShopItemWidget : public UItemWidget, public IUserObjectListEntry, public ITreeNodeInterface
{
	GENERATED_BODY()

public:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

	//~Begin ITreeNodeInterface interface.
	virtual UUserWidget* GetWidget() const override;
	virtual TArray<const ITreeNodeInterface*> GetInputs() const override;
	virtual TArray<const ITreeNodeInterface*> GetOutputs() const override;
	virtual const UObject* GetItemObject() const override;
	//~End ITreeNodeInterface interface.

	FOnShopItemPurchaseIssued OnShopItemPurchaseIssued;
	FOnShopItemSelected OnShopItemSelected;

private:
	UPROPERTY()
	const UPA_ShopItem* ShopItem;

	UPROPERTY()
	const UListView* ParentListView;

	virtual void RightButtonClicked() override;
	virtual void LeftButtonClicked() override;

	void CopyFromOther(const UShopItemWidget* OtherWidget);
	void InitWithShopItem(const UPA_ShopItem* NewShopItem);
	TArray<const ITreeNodeInterface*> ItemsToInterfaces(const TArray<const UPA_ShopItem*>& Items) const;

public:
	FORCEINLINE const UPA_ShopItem* GetShopItem() const { return ShopItem; }
};

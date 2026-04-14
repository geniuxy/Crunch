#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "Inventory/Data/PA_ShopItem.h"
#include "CStruct.generated.h"

class UPA_ShopItem;

USTRUCT(BlueprintType)
struct FGenericDamageEffectDef
{
	GENERATED_BODY()

	FGenericDamageEffectDef(): DamageEffect(nullptr), PushVel(0.f)
	{
	}

	UPROPERTY(EditAnywhere)
	TSubclassOf<UGameplayEffect> DamageEffect;

	UPROPERTY(EditAnywhere)
	FVector PushVel;
};

USTRUCT(BlueprintType)
struct FAbilityWidgetData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UGameplayAbility> AbilityClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName AbilityName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UTexture2D> Icon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Description;
};

USTRUCT(BlueprintType)
struct FHeroBaseStats : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> HeroClass;

	UPROPERTY(EditAnywhere)
	float Strength = 0.f;

	UPROPERTY(EditAnywhere)
	float Intelligence = 0.f;

	UPROPERTY(EditAnywhere)
	float StrengthGrowthRate = 0.f;

	UPROPERTY(EditAnywhere)
	float IntelligenceGrowthRate = 0.f;

	UPROPERTY(EditAnywhere)
	float BaseMaxHealth = 0.f;

	UPROPERTY(EditAnywhere)
	float BaseMaxMana = 0.f;

	UPROPERTY(EditAnywhere)
	float BaseAttackDamage = 0.f;

	UPROPERTY(EditAnywhere)
	float BaseArmor = 0.f;

	UPROPERTY(EditAnywhere)
	float BaseMoveSpeed = 0.f;
};

USTRUCT(BlueprintType)
struct FItemCollection
{
	GENERATED_BODY()

public:
	FItemCollection()
	{
	}

	FItemCollection(const TArray<const UPA_ShopItem*>& InItems): Items(InItems)
	{
	}

	void AddItem(const UPA_ShopItem* NewItem, bool bAddUnique = false)
	{
		if (bAddUnique && Contains(NewItem)) return;

		Items.Add(NewItem);
	}

	bool Contains(const UPA_ShopItem* Item) const
	{
		return Items.Contains(Item);
	}

	const TArray<const UPA_ShopItem*>& GetItems() const
	{
		return Items;
	}

private:
	UPROPERTY()
	TArray<const UPA_ShopItem*> Items;
};

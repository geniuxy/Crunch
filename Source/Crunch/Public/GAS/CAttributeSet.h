// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "CoreMinimal.h"
#include "CAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)
/**
 * Tips: 控制台输入 AbilitySystem.DebugAttribute Health MaxHealth (可以实时debug)
 */
UCLASS()
class CRUNCH_API UCAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	ATTRIBUTE_ACCESSORS(UCAttributeSet, Health)
	ATTRIBUTE_ACCESSORS(UCAttributeSet, MaxHealth)
	ATTRIBUTE_ACCESSORS(UCAttributeSet, Mana)
	ATTRIBUTE_ACCESSORS(UCAttributeSet, MaxMana)
	ATTRIBUTE_ACCESSORS(UCAttributeSet, AttackDamage)
	ATTRIBUTE_ACCESSORS(UCAttributeSet, Armor)
	ATTRIBUTE_ACCESSORS(UCAttributeSet, MoveSpeed)

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	// 用于GameplayEffect的Modifiers相关计算Clamp
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	// 用于GameplayEffect的Executions相关计算Clamp
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

private:
	UPROPERTY(ReplicatedUsing = OnRep_Health)
	FGameplayAttributeData Health;
	UPROPERTY(ReplicatedUsing = OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth;
	UPROPERTY(ReplicatedUsing = OnRep_Mana)
	FGameplayAttributeData Mana;
	UPROPERTY(ReplicatedUsing = OnRep_MaxMana)
	FGameplayAttributeData MaxMana;
	UPROPERTY(ReplicatedUsing = OnRep_AttackDamage)
	FGameplayAttributeData AttackDamage;
	UPROPERTY(ReplicatedUsing = OnRep_Armor)
	FGameplayAttributeData Armor;
	UPROPERTY(ReplicatedUsing = OnRep_MoveSpeed)
	FGameplayAttributeData MoveSpeed;

	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_Mana(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MaxMana(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_AttackDamage(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_Armor(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MoveSpeed(const FGameplayAttributeData& OldValue);
};

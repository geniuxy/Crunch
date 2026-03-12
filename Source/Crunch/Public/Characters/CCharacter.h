// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "CCharacter.generated.h"

class UCAttributeSet;
class UCAbilitySystemComponent;
class UWidgetComponent;

UCLASS()
class CRUNCH_API ACCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ACCharacter();

	void ServerSideInit();
	void ClientSideInit();

protected:
	virtual void BeginPlay() override;

	/**********************************************************************/
	/*                         Gameplay Ability                           */
	/**********************************************************************/
public:
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

private:
	UPROPERTY(VisibleDefaultsOnly, Category = "Gameplay Ability")
	UCAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY()
	UCAttributeSet* AttributeSet;

	/**********************************************************************/
	/*                                UI                                  */
	/**********************************************************************/
public:
	void ConfigureOverHeadStatsWidget();
private:
	UPROPERTY(VisibleDefaultsOnly, Category="UI")
	UWidgetComponent* OverHeadWidgetComponent;
};

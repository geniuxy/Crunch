// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Blueprint/UserWidget.h"
#include "CharacterEntryWidget.generated.h"

class UPA_CharacterDefinition;
class UTextBlock;
class UImage;
/**
 * 
 */
UCLASS()
class CRUNCH_API UCharacterEntryWidget : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

public:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

	void SetSelected(bool bIsSelected);

private:
	UPROPERTY(meta=(BindWidget))
	UImage* CharacterIcon;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* CharacterNameText;

	UPROPERTY(EditDefaultsOnly, Category="Character")
	FName IconTextureMatParamName = "Icon";

	UPROPERTY(EditDefaultsOnly, Category="Character")
	FName SaturationMatParamName = "Saturation";

	UPROPERTY()
	const UPA_CharacterDefinition* CharacterDefinition;

public:
	FORCEINLINE const UPA_CharacterDefinition* GetCharacterDefinition() const { return CharacterDefinition; }
};

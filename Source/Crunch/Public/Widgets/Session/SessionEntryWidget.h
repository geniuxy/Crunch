// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SessionEntryWidget.generated.h"

class UButton;
class UTextBlock;
DECLARE_MULTICAST_DELEGATE_OneParam(FOnSessionEntrySelected, const FString& /* SelectedSessionIDStr */)
/**
 * 
 */
UCLASS()
class CRUNCH_API USessionEntryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	FOnSessionEntrySelected OnSessionEntrySelected;

	void InitializeEntry(const FString& Name, const FString& SessionIDStr);

private:
	UPROPERTY(meta=(BindWidget))
	UTextBlock* SessionNameText;

	UPROPERTY(meta=(BindWidget))
	UButton* SessionButton;
	
	FString CachedSessionIDStr;

	UFUNCTION()
	void SessionEntrySelected();

public:
	FORCEINLINE FString GetCachedSessionIDStr() const { return CachedSessionIDStr; }
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayMenu.generated.h"

class UButton;
class UTextBlock;
class FOnButtonClickedEvent;
/**
 * 
 */
UCLASS()
class CRUNCH_API UGameplayMenu : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	FOnButtonClickedEvent& GetResumeButtonClickedEventDelegate();

private:
	UPROPERTY(meta=(BindWidget))
	UTextBlock* MenuTitle;
	
	UPROPERTY(meta=(BindWidget))
	UButton* ResumeButton;
	
	UPROPERTY(meta=(BindWidget))
	UButton* MainMenuButton;
	
	UPROPERTY(meta=(BindWidget))
	UButton* QuitGameButton;

	UFUNCTION()
	void BackToMainMenu();

	UFUNCTION()
	void QuitGame();
};

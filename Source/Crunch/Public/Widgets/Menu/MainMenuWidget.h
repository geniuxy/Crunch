// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuWidget.generated.h"

class FOnButtonClickedEvent;
class UWaitingWidget;
class UButton;
class UCGameInstance;
class UWidgetSwitcher;
/**
 * 
 */
UCLASS()
class CRUNCH_API UMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	/**********************************************************************/
	/*                              Main                                  */
	/**********************************************************************/
private:
	UPROPERTY(meta=(BindWidget))
	UWidgetSwitcher* MainSwitcher;

	UPROPERTY()
	UCGameInstance* CGameInstance;

	void SwitchToMainWidget();

	UPROPERTY(meta=(BindWidget))
	UWidget* MainWidgetRoot;

	/**********************************************************************/
	/*                              Login                                 */
	/**********************************************************************/
private:
	UPROPERTY(meta=(BindWidget))
	UWidget* LoginWidgetRoot;

	UPROPERTY(meta=(BindWidget))
	UButton* LoginButton;

	UFUNCTION()
	void LoginButtonClicked();

	void LoginCompleted(bool bWasSuccessful, const FString& PlayerNickName, const FString& ErrorMsg);

	/**********************************************************************/
	/*                             Waiting                                */
	/**********************************************************************/
private:
	UPROPERTY(meta=(BindWidget))
	UWaitingWidget* WaitingWidget;

	FOnButtonClickedEvent& SwitchToWaitingWidget(const FText& WaitingInfo, bool bAllowCancel = false);
};

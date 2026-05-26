// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuWidget.generated.h"

class USessionEntryWidget;
class UScrollBox;
class UEditableText;
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
	/*                             Session                                */
	/**********************************************************************/
private:
	UPROPERTY(meta=(BindWidget))
	UButton* CreateSessionButton;

	UPROPERTY(meta=(BindWidget))
	UEditableText* NewSessionNameText;

	UFUNCTION()
	void CreateSessionButtonClicked();

	UFUNCTION()
	void CancelSessionCreation();

	UFUNCTION()
	void NewSessionNameTextChanged(const FText& NewText);

	void JoinSessionFailed();

	void UpdateLobbyList(const TArray<FOnlineSessionSearchResult>& SearchResults);

	UPROPERTY(meta=(BindWidget))
	UScrollBox* SessionScrollBox;
	
	UPROPERTY(meta=(BindWidget))
	UButton* JoinSessionButton;

	UPROPERTY(EditDefaultsOnly, Category="Session")
	TSubclassOf<USessionEntryWidget> SessionEntryWidgetClass;

	FString CurrentSelectedSessionID = "";

	UFUNCTION()
	void JoinSessionButtonClicked();

	void SessionEntrySelected(const FString& SelectedEntryIDStr);

	/**********************************************************************/
	/*                              Login                                 */
	/**********************************************************************/
private:
	UPROPERTY(meta=(BindWidget))
	UWidget* LoginWidgetRoot;

	UPROPERTY(meta=(BindWidget))
	UButton* LoginButton;
	
	void SwitchToLoginWidget();

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

// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Menu/MainMenuWidget.h"

#include "CrunchDebugHelper.h"
#include "OnlineSessionSettings.h"
#include "Components/Button.h"
#include "Components/EditableText.h"
#include "Components/ScrollBox.h"
#include "Components/WidgetSwitcher.h"
#include "FrameWork/CGameInstance.h"
#include "FunctionLibrary/NetFunctionLibrary.h"
#include "Widgets/Menu/WaitingWidget.h"
#include "Widgets/Session/SessionEntryWidget.h"

void UMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	CGameInstance = GetGameInstance<UCGameInstance>();
	if (CGameInstance)
	{
		CGameInstance->OnLoginCompleted.AddUObject(this, &ThisClass::LoginCompleted);
		if (CGameInstance->IsLoggedIn())
		{
			SwitchToMainWidget();
		}

		CGameInstance->OnJoinSessionFailed.AddUObject(this, &ThisClass::JoinSessionFailed);
		CGameInstance->OnGlobalSessionSearchCompleted.AddUObject(this, &ThisClass::UpdateLobbyList);
	}

	LoginButton->OnClicked.AddDynamic(this, &ThisClass::LoginButtonClicked);

	CreateSessionButton->OnClicked.AddDynamic(this, &ThisClass::CreateSessionButtonClicked);
	CreateSessionButton->SetIsEnabled(false);

	NewSessionNameText->OnTextChanged.AddDynamic(this, &ThisClass::NewSessionNameTextChanged);
	JoinSessionButton->OnClicked.AddDynamic(this, &ThisClass::JoinSessionButtonClicked);
	JoinSessionButton->SetIsEnabled(false);
}

void UMainMenuWidget::SwitchToMainWidget()
{
	if (MainSwitcher)
	{
		MainSwitcher->SetActiveWidget(MainWidgetRoot);
	}

	// 切换到主界面就开始全局房间会话搜索
	if (CGameInstance)
	{
		CGameInstance->StartGlobalSessionSearch();
	}
}

void UMainMenuWidget::CreateSessionButtonClicked()
{
	if (CGameInstance && CGameInstance->IsLoggedIn())
	{
		CGameInstance->RequestCreateAndJoinSession(FName(NewSessionNameText->GetText().ToString()));
		SwitchToWaitingWidget(FText::FromString(TEXT("创建房间中")), true).AddDynamic(
			this, &ThisClass::CancelSessionCreation
		);
	}
}

void UMainMenuWidget::CancelSessionCreation()
{
	if (CGameInstance)
	{
		CGameInstance->CancelSessionCreation();
	}
	SwitchToMainWidget();
}

void UMainMenuWidget::NewSessionNameTextChanged(const FText& NewText)
{
	CreateSessionButton->SetIsEnabled(!NewText.IsEmpty());
}

void UMainMenuWidget::JoinSessionFailed()
{
	SwitchToMainWidget();
}

void UMainMenuWidget::UpdateLobbyList(const TArray<FOnlineSessionSearchResult>& SearchResults)
{
	Debug::Print(TEXT("根据搜索会话结果，更新会话列表"));

	SessionScrollBox->ClearChildren();

	bool bCurrentSelectedSessionValid = false;
	for (const FOnlineSessionSearchResult& SearchResult : SearchResults)
	{
		USessionEntryWidget* NewSessionWidget =
			CreateWidget<USessionEntryWidget>(GetOwningPlayer(), SessionEntryWidgetClass);
		if (NewSessionWidget)
		{
			FString SessionName = "Name_None";
			SearchResult.Session.SessionSettings.Get<FString>(UNetFunctionLibrary::GetSessionNameKey(), SessionName);

			FString SessionIDStr = SearchResult.Session.GetSessionIdStr();
			NewSessionWidget->InitializeEntry(SessionName, SessionIDStr);
			NewSessionWidget->OnSessionEntrySelected.AddUObject(this, &ThisClass::SessionEntrySelected);
			SessionScrollBox->AddChild(NewSessionWidget);
			if (CurrentSelectedSessionID == SessionIDStr)
			{
				bCurrentSelectedSessionValid = true;
			}
		}
	}

	CurrentSelectedSessionID = bCurrentSelectedSessionValid ? CurrentSelectedSessionID : "";
	JoinSessionButton->SetIsEnabled(bCurrentSelectedSessionValid);
}

void UMainMenuWidget::JoinSessionButtonClicked()
{
	if (CGameInstance && !CurrentSelectedSessionID.IsEmpty())
	{
		Debug::Print(TEXT("尝试加入会话，ID"), *CurrentSelectedSessionID);
		if (CGameInstance->JoinSessionWithID(CurrentSelectedSessionID))
		{
			SwitchToWaitingWidget(FText::FromString(TEXT("加入房间中")));
		}
	}
	else
	{
		Debug::Print(TEXT("当前不能加入会话，没有会话被选择"));
	}
}

void UMainMenuWidget::SessionEntrySelected(const FString& SelectedEntryIDStr)
{
	CurrentSelectedSessionID = SelectedEntryIDStr;
}

void UMainMenuWidget::LoginButtonClicked()
{
	Debug::Print(TEXT("正在登录中！"));
	if (CGameInstance && !CGameInstance->IsLoggedIn() && !CGameInstance->IsLoggingIn())
	{
		CGameInstance->ClientAccountPortalLogin();
		SwitchToWaitingWidget(FText::FromString(TEXT("正在登入中")));
	}
}

void UMainMenuWidget::LoginCompleted(bool bWasSuccessful, const FString& PlayerNickName, const FString& ErrorMsg)
{
	if (bWasSuccessful)
	{
		Debug::Print(TEXT("登录成功！"));
	}
	else
	{
		Debug::Print(TEXT("登录失败！"));
	}

	SwitchToMainWidget();
}

FOnButtonClickedEvent& UMainMenuWidget::SwitchToWaitingWidget(const FText& WaitingInfo, bool bAllowCancel)
{
	MainSwitcher->SetActiveWidget(WaitingWidget);
	WaitingWidget->SetWaitingInfo(WaitingInfo, bAllowCancel);
	return WaitingWidget->ClearAndGetButtonClickedEvent();
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Menu/MainMenuWidget.h"

#include "CrunchDebugHelper.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "FrameWork/CGameInstance.h"
#include "Widgets/Menu/WaitingWidget.h"

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
	}

	LoginButton->OnClicked.AddDynamic(this, &ThisClass::LoginButtonClicked);
}

void UMainMenuWidget::SwitchToMainWidget()
{
	if (MainSwitcher)
	{
		MainSwitcher->SetActiveWidget(MainWidgetRoot);
	}
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

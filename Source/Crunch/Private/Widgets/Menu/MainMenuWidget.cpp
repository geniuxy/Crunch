// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Menu/MainMenuWidget.h"

#include "CrunchDebugHelper.h"
#include "Components/Button.h"
#include "FrameWork/CGameInstance.h"

void UMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	CGameInstance = GetGameInstance<UCGameInstance>();
	if (CGameInstance)
	{
		
	}

	LoginButton->OnClicked.AddDynamic(this, &ThisClass::LoginButtonClicked);
}

void UMainMenuWidget::LoginButtonClicked()
{
	Debug::Print(TEXT("正在登录中！"));
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
}

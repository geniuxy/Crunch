// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Menu/GameplayMenu.h"

#include "Components/Button.h"
#include "Kismet/KismetSystemLibrary.h"

void UGameplayMenu::NativeConstruct()
{
	Super::NativeConstruct();

	MainMenuButton->OnClicked.AddDynamic(this, &ThisClass::BackToMainMenu);
	QuitGameButton->OnClicked.AddDynamic(this, &ThisClass::QuitGame);
}

FOnButtonClickedEvent& UGameplayMenu::GetResumeButtonClickedEventDelegate()
{
	return ResumeButton->OnClicked;
}

void UGameplayMenu::BackToMainMenu()
{
	
}

void UGameplayMenu::QuitGame()
{
	UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, true);
}

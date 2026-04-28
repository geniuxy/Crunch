// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Stats/MatchStatWidget.h"

#include "Characters/CStormCore.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"

void UMatchStatWidget::NativeConstruct()
{
	Super::NativeConstruct();

	StormCore = Cast<ACStormCore>(UGameplayStatics::GetActorOfClass(this, ACStormCore::StaticClass()));
	if (StormCore)
	{
		StormCore->OnTeamInfluencerCountUpdatedDelegate.AddUObject(this, &ThisClass::UpdateTeamInfluence);
		StormCore->OnGoalReachedDelegate.AddUObject(this, &ThisClass::MatchFinished);
		GetWorld()->GetTimerManager().SetTimer(
			UpdateProgressTimerHandle, this, &UMatchStatWidget::UpdateProgress, ProgressUpdateInterval, true
		);
	}
}

void UMatchStatWidget::UpdateTeamInfluence(int TeamOneCount, int TeamTwoCount)
{
	TeamOneCountText->SetText(FText::AsNumber(TeamOneCount));
	TeamTwoCountText->SetText(FText::AsNumber(TeamTwoCount));
}

void UMatchStatWidget::MatchFinished(AActor* ViewTarget, int WinningTeam)
{
	float Progress = WinningTeam == 0 ? 1 : 0;

	GetWorld()->GetTimerManager().ClearTimer(UpdateProgressTimerHandle);
	ProgressImage->GetDynamicMaterial()->SetScalarParameterValue(ProgressDynamicMaterialParamName, Progress);
}

void UMatchStatWidget::UpdateProgress()
{
	if (StormCore)
	{
		float Progress = StormCore->GetProgress();
		ProgressImage->GetDynamicMaterial()->SetScalarParameterValue(ProgressDynamicMaterialParamName, Progress);
	}
}

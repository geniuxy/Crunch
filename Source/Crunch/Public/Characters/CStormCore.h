// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CStormCore.generated.h"

class UCameraComponent;
class AAIController;
class USphereComponent;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnGoalReachedDelegate, AActor* /* ViewTarget */, int /* WinningTeam */)
DECLARE_MULTICAST_DELEGATE_TwoParams(
	FOnTeamInfluencerCountUpdatedDelegate, int /* TeamOneInfluencerCount */, int /* TeamTwoInfluencerCount*/)

UCLASS()
class CRUNCH_API ACStormCore : public ACharacter
{
	GENERATED_BODY()

public:
	ACStormCore();

	FOnGoalReachedDelegate OnGoalReachedDelegate;
	FOnTeamInfluencerCountUpdatedDelegate OnTeamInfluencerCountUpdatedDelegate;

	float GetProgress() const;

protected:
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void Tick(float DeltaSeconds) override;

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

private:
	UPROPERTY(EditAnywhere, Category="Move")
	float MaxWalkSpeed = 500.f;

	UPROPERTY(EditAnywhere, Category="Move")
	UAnimMontage* ExpandMontage;

	UPROPERTY(EditAnywhere, Category="Move")
	UAnimMontage* CaptureMontage;

	UPROPERTY(EditAnywhere, Category="Detection")
	float InfluenceRadius = 1000.f;

	UPROPERTY(VisibleDefaultsOnly, Category="Detection")
	USphereComponent* InfluenceRange;

	UPROPERTY(VisibleDefaultsOnly, Category="Detection")
	UDecalComponent* GroundDecalComponent;

	UPROPERTY(VisibleDefaultsOnly, Category="Detection")
	UCameraComponent* ViewCam;

	UFUNCTION()
	void NewInfluencerInRange(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	UFUNCTION()
	void InfluencerLeftRange(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);

	void UpdateTeamWeight();
	void UpdateGoal();

	UPROPERTY(EditAnywhere, Category="Team")
	AActor* TeamOneGoal; // Team 1 小兵和StormCore的目标点

	UPROPERTY(EditAnywhere, Category="Team")
	AActor* TeamTwoGoal; // Team 2 小兵和StormCore的目标点

	UPROPERTY(EditAnywhere, Category="Team")
	AActor* TeamOneCore; // Team 1 的基地

	UPROPERTY(EditAnywhere, Category="Team")
	AActor* TeamTwoCore; // Team 2 的基地

	UPROPERTY(ReplicatedUsing=OnRep_CoreToCapture)
	AActor* CoreToCapture;

	UFUNCTION()
	void OnRep_CoreToCapture();

	float CoreCaptureSpeed = 0.f; // 捕获目标基地的播放速度
	float TravelLength = 0.f; // 双方目标点Goal之间的距离

	void GoalReached(int WinningTeam);

	void CaptureCore();
	void ExpandFinished();

	int TeamOneInfluencerCount = 0;
	int TeamTwoInfluencerCount = 0;

	float TeamWeight = 0.f;

	UPROPERTY()
	AAIController* OwnerAIController;
};

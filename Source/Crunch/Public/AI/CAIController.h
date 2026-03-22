// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "CAIController.generated.h"

class UAISenseConfig_Sight;
/**
 * 
 */
UCLASS()
class CRUNCH_API ACAIController : public AAIController
{
	GENERATED_BODY()

public:
	ACAIController();

protected:
	virtual void OnPossess(APawn* NewPawn) override;
	
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly, Category="AI Behavior")
	FName TargetBlackboardKeyName = FName("Target");
	
	UPROPERTY(EditDefaultsOnly, Category="AI Behavior")
	UBehaviorTree* BehaviorTree;
	
	UPROPERTY(VisibleAnywhere, Category="Perception")
	UAIPerceptionComponent* AIPerceptionComponent;

	UPROPERTY(VisibleAnywhere, Category="Perception")
	UAISenseConfig_Sight* SightConfig;
	
	UFUNCTION()
	void HandleTargetPerceptionUpdated(AActor* TargetActor, FAIStimulus Stimulus);
	
	UFUNCTION()
	void HandleTargetPerceptionForgotten(AActor* ForgottenActor);

	const UObject* GetCurrentTarget() const;

	void SetCurrentTarget(AActor* NewTarget);

	AActor* GetNextPerceivedActor() const;
};

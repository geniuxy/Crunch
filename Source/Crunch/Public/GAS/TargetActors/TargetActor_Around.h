// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "TargetActor_Around.generated.h"

class USphereComponent;

UCLASS()
class CRUNCH_API ATargetActor_Around : public AGameplayAbilityTargetActor, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	ATargetActor_Around();

	void ConfigureDetection(
		float DetectionRadius, const FGenericTeamId& InTeamID, const FGameplayTag& InLocalGameplayCueTag
	);

	virtual void SetGenericTeamId(const FGenericTeamId& InTeamID) override;
	FORCEINLINE virtual FGenericTeamId GetGenericTeamId() const override { return TeamID; }

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	UPROPERTY(VisibleDefaultsOnly, Category="Comp")
	USceneComponent* RootComp;

	UPROPERTY(VisibleDefaultsOnly, Category="Targeting")
	USphereComponent* DetectionSphere;

	UPROPERTY(Replicated)
	FGenericTeamId TeamID;

	UPROPERTY(ReplicatedUsing=OnRep_TargetDetectionRadiusReplicated)
	float TargetDetectionRadius;

	UPROPERTY(Replicated)
	FGameplayTag LocalGameplayCueTag;

	UFUNCTION()
	void OnRep_TargetDetectionRadiusReplicated();

	UFUNCTION()
	void ActorInDetectionRange(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);
};

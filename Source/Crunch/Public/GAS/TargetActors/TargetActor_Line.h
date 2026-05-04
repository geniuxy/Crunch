// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "TargetActor_Line.generated.h"

class UNiagaraComponent;
class USphereComponent;

UCLASS()
class CRUNCH_API ATargetActor_Line : public AGameplayAbilityTargetActor, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	ATargetActor_Line();

	void ConfigureTargetSetting(
		float NewTargetRange,
		float NewDetectionCylinderRadius,
		float NewTargetingInterval,
		FGenericTeamId OwnerTeamID,
		bool bShouldDrawDebug
	);

	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override { TeamID = NewTeamID; }
	FORCEINLINE virtual FGenericTeamId GetGenericTeamId() const override { return TeamID; }

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	UPROPERTY(VisibleDefaultsOnly, Category="Component")
	USceneComponent* RootComp;

	UPROPERTY(VisibleDefaultsOnly, Category="Component")
	USphereComponent* TargetEndDetectionSphere;

	UPROPERTY(VisibleDefaultsOnly, Category="Component")
	UNiagaraComponent* LaserVFX;

	UPROPERTY(Replicated)
	float TargetRange;

	UPROPERTY(Replicated)
	float DetectionCylinderRadius;

	UPROPERTY()
	float TargetingInterval;

	UPROPERTY(Replicated)
	FGenericTeamId TeamID;

	UPROPERTY()
	bool bDrawDebug;

	UPROPERTY(Replicated)
	const AActor* AvatarActor;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "TargetActor_BlackHole.generated.h"

class UNiagaraSystem;
class UNiagaraComponent;
class USphereComponent;

UCLASS()
class CRUNCH_API ATargetActor_BlackHole : public AGameplayAbilityTargetActor, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	ATargetActor_BlackHole();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void ConfigureBlackHole(
		float InBlackHoleRange, float InPullSpeed, float InBlackHoleDuration, const FGenericTeamId& InTeamID
	);
	void SetBlackHoleLinkOrigin();

	virtual void SetGenericTeamId(const FGenericTeamId& InTeamID) override { TeamID = InTeamID; }
	virtual FGenericTeamId GetGenericTeamId() const override { return TeamID; }

	virtual void StartTargeting(UGameplayAbility* Ability) override;

	virtual void Tick(float DeltaSeconds) override;

	virtual void ConfirmTargetingAndContinue() override;

	virtual void CancelTargeting() override;

private:
	UPROPERTY(Replicated)
	FGenericTeamId TeamID;

	float PullSpeed;
	float BlackHoleDuration;

	UPROPERTY(ReplicatedUsing=OnRep_BlackHoleRange)
	float BlackHoleRange;

	UFUNCTION()
	void OnRep_BlackHoleRange();

	UPROPERTY(VisibleDefaultsOnly, Category="Component")
	USceneComponent* RootComp;

	UPROPERTY(VisibleDefaultsOnly, Category="Component")
	USphereComponent* DetectionSphereComponent;

	UPROPERTY(VisibleDefaultsOnly, Category="Component")
	UParticleSystemComponent* VFXComponent;

	FTimerHandle BlackHoleDurationTimerHandle;

	UPROPERTY(EditDefaultsOnly, Category="VFX")
	FName BlackHoleVFXOriginVariableName = "Origin";
	
	UPROPERTY(EditDefaultsOnly, Category="VFX")
	UNiagaraSystem* BlackHoleLinkVFX;

	UFUNCTION()
	void ActorInBlackHoleRange(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	UFUNCTION()
	void ActorLeftBlackHoleRange(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int OtherBodyIndex
	);

	void TryAddTarget(AActor* OtherTarget);
	void RemoveTarget(AActor* OtherTarget);

	UPROPERTY()
	TMap<AActor*, UNiagaraComponent*> ActorsInRangeMap;

	void StopBlackHole();
};

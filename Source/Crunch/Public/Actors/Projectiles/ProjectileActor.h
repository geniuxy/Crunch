// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "GenericTeamAgentInterface.h"
#include "GameFramework/Actor.h"
#include "ProjectileActor.generated.h"

UCLASS()
class CRUNCH_API AProjectileActor : public AActor, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	AProjectileActor();

	virtual void Tick(float DeltaSeconds) override;

	void ShootProjectile(
		float InSpeed,
		float InMaxDistance,
		const AActor* InTarget,
		FGenericTeamId InTeamID,
		FGameplayEffectSpecHandle InHitEffectHandle
	);

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;

	virtual FGenericTeamId GetGenericTeamId() const override;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	
protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly, Category="Gameplay Cue")
	FGameplayTag HitGameplayCueTag;
	
	UPROPERTY(Replicated)
	FGenericTeamId TeamID;
	
	UPROPERTY(Replicated)
	FVector MoveDir;
	
	UPROPERTY(Replicated)
	float ProjectileSpeed;

	UPROPERTY()
	const AActor* Target;
	
	FGameplayEffectSpecHandle HitEffectSpecHandle;
	FTimerHandle ShootTimerHandle;
	
	void TravelMaxDistanceReached();

	void SendLocalGameplayCue(AActor* CurTargetActor, const FHitResult& HitResult);
};

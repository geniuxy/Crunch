// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "TargetActor_Line.generated.h"

class UNiagaraComponent;
class USphereComponent;

UCLASS()
class CRUNCH_API ATargetActor_Line : public AGameplayAbilityTargetActor
{
	GENERATED_BODY()

public:
	ATargetActor_Line();

private:
	UPROPERTY(VisibleDefaultsOnly, Category="Component")
	USceneComponent* RootComp;

	UPROPERTY(VisibleDefaultsOnly, Category="Component")
	USphereComponent* TargetEndDetectionSphere;
	
	UPROPERTY(VisibleDefaultsOnly, Category="Component")
	UNiagaraComponent* LaserVFX;

	const AActor* AvatarActor;
};

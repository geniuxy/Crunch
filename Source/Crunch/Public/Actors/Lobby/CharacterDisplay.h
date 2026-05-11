// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CharacterDisplay.generated.h"

class UCameraComponent;
class UPA_CharacterDefinition;

UCLASS()
class CRUNCH_API ACharacterDisplay : public AActor
{
	GENERATED_BODY()

public:
	ACharacterDisplay();

	void ConfigureWithCharacterDefinition(const UPA_CharacterDefinition* CharacterDefinition);

private:
	UPROPERTY(VisibleDefaultsOnly, Category="Character Display")
	USkeletalMeshComponent* MeshComponent;
	
	UPROPERTY(VisibleDefaultsOnly, Category="Character Display")
	UCameraComponent* ViewCameraComponent;
};

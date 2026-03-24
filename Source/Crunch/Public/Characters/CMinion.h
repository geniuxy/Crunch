// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CCharacter.h"
#include "CMinion.generated.h"

UCLASS()
class CRUNCH_API ACMinion : public ACCharacter
{
	GENERATED_BODY()

protected:
	virtual void SetGenericTeamId(const FGenericTeamId& InTeamID) override;
	
	virtual void OnRep_TeamID() override;

private:
	void PickSkinBasedOnTeamID();

	UPROPERTY(EditDefaultsOnly, Category="Visual")
	TMap<FGenericTeamId, USkeletalMesh*> SkinMap;
};

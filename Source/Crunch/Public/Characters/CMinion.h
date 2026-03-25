// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CCharacter.h"
#include "CMinion.generated.h"

UCLASS()
class CRUNCH_API ACMinion : public ACCharacter
{
	GENERATED_BODY()

public:
	virtual void SetGenericTeamId(const FGenericTeamId& InTeamID) override;

	bool IsActive() const;
	void Activate();
	
protected:
	virtual void OnRep_TeamID() override;

private:
	void PickSkinBasedOnTeamID();

	UPROPERTY(EditDefaultsOnly, Category="Visual")
	TMap<FGenericTeamId, USkeletalMesh*> SkinMap;
};

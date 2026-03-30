#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "CStruct.generated.h"

USTRUCT(BlueprintType)
struct FGenericDamageEffectDef
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	TSubclassOf<UGameplayEffect> DamageEffect;

	UPROPERTY(EditAnywhere)
	FVector PushVel;
};

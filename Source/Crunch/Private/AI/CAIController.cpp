// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/CAIController.h"

#include "AbilitySystemComponent.h"
#include "CGameplayTags.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "FunctionLibrary/CAbilitySystemFunctionLibrary.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"

/*
 * ' 号可以查看单位的ai信息，Num5可以查看
 */
ACAIController::ACAIController()
{
	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>("AI Perception Component");
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>("Sight Config");

	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = false;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = false;

	SightConfig->SightRadius = 1000.f;
	SightConfig->LoseSightRadius = 1200.f;

	SightConfig->SetMaxAge(5.f); // 跑出检测范围后，仍然记得对应敌人的时间

	SightConfig->PeripheralVisionAngleDegrees = 180.f; // AI可看到的侧向范围

	AIPerceptionComponent->ConfigureSense(*SightConfig);
	AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &ThisClass::HandleTargetPerceptionUpdated);
	// 要实现OnTargetPerceptionForgotten的触发，需要打开设置中的Forget Stale Actors
	AIPerceptionComponent->OnTargetPerceptionForgotten.AddDynamic(this, &ThisClass::HandleTargetPerceptionForgotten);
}

void ACAIController::OnPossess(APawn* NewPawn)
{
	Super::OnPossess(NewPawn);
	SetGenericTeamId(FGenericTeamId(0));

	IGenericTeamAgentInterface* PawnTeamInterface = Cast<IGenericTeamAgentInterface>(NewPawn);
	if (PawnTeamInterface)
	{
		PawnTeamInterface->SetGenericTeamId(GetGenericTeamId());
	}
}

void ACAIController::BeginPlay()
{
	Super::BeginPlay();

	RunBehaviorTree(BehaviorTree);
}

void ACAIController::HandleTargetPerceptionUpdated(AActor* TargetActor, FAIStimulus Stimulus)
{
	if (Stimulus.WasSuccessfullySensed())
	{
		if (!GetCurrentTarget())
		{
			SetCurrentTarget(TargetActor);
		}
	}
	else
	{
		// 要做到AI感应的敌人死了以后，AI感知不到
		// 1. 需要Character死了之后关闭AI感知刺激源Comp；2. AIController感知源变化时，将对应Actor的阈值调最大以清除
		ForgetActorIfDead(TargetActor);
	}
}

void ACAIController::HandleTargetPerceptionForgotten(AActor* ForgottenActor)
{
	if (!ForgottenActor) return;

	if (GetCurrentTarget() == ForgottenActor)
	{
		SetCurrentTarget(GetNextPerceivedActor());
	}
}

const UObject* ACAIController::GetCurrentTarget() const
{
	const UBlackboardComponent* BlackboardComponent = GetBlackboardComponent();
	if (BlackboardComponent)
	{
		return BlackboardComponent->GetValueAsObject(TargetBlackboardKeyName);
	}
	return nullptr;
}

void ACAIController::SetCurrentTarget(AActor* NewTarget)
{
	UBlackboardComponent* BlackboardComponent = GetBlackboardComponent();
	if (!BlackboardComponent) return;

	if (NewTarget)
	{
		BlackboardComponent->SetValueAsObject(TargetBlackboardKeyName, NewTarget);
	}
	else
	{
		BlackboardComponent->ClearValue(TargetBlackboardKeyName);
	}
}

AActor* ACAIController::GetNextPerceivedActor() const
{
	if (AIPerceptionComponent)
	{
		TArray<AActor*> Actors;
		AIPerceptionComponent->GetPerceivedHostileActors(Actors);

		if (Actors.Num() != 0)
		{
			return Actors[0];
		}
	}
	return nullptr;
}

void ACAIController::ForgetActorIfDead(AActor* ActorToForget)
{
	const UAbilitySystemComponent* ActorASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(ActorToForget);
	if (!ActorASC) return;

	// 当检测到目标 Actor 死亡时，强制让 AI 立即"遗忘"该目标
	if (ActorASC->HasMatchingGameplayTag(CGameplayTags::Crunch_Stats_Dead))
	{
		for (UAIPerceptionComponent::TActorPerceptionContainer::TIterator
		     Iter = AIPerceptionComponent->GetPerceptualDataIterator(); Iter; ++Iter)
		{
			if (Iter->Key != ActorToForget)
			{
				continue;
			}

			for (FAIStimulus& Stimulus : Iter->Value.LastSensedStimuli)
			{
				// 把遗忘阈值调到最大 == 自动清除
				Stimulus.SetStimulusAge(TNumericLimits<float>::Max());
			}
		}
	}
}

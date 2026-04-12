// Fill out your copyright notice in the Description page of Project Settings.


#include "Crunch/Public/Characters/CCharacter.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "CGameplayTags.h"
#include "CrunchDebugHelper.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "Crunch/Crunch.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerState.h"
#include "GAS/CAbilitySystemComponent.h"
#include "GAS/CAttributeSet.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"
#include "Widgets/OverHeadStatsGauge.h"


ACCharacter::ACCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	// 总是会生成
	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	// 需要把Mesh改成NoCollision,其余的Responses都可以改为Block
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionResponseToAllChannels(ECR_Block);

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore); // CapsuleComponent不要挡摄像头
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Target, ECR_Ignore); // CapsuleComponent不要挡目标跟踪通道

	AbilitySystemComponent = CreateDefaultSubobject<UCAbilitySystemComponent>(TEXT("Ability System Component"));
	AttributeSet = CreateDefaultSubobject<UCAttributeSet>(TEXT("AttributeSet"));

	OverHeadWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("Over Head Widget Component"));
	OverHeadWidgetComponent->SetupAttachment(GetRootComponent());

	BindGASChangeDelegates();

	PerceptionStimuliSourceComponent =
		CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("Perception Stimuli Source Component"));
}

void ACCharacter::ServerSideInit()
{
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	AbilitySystemComponent->ServerSideInit();
}

void ACCharacter::ClientSideInit()
{
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
}

bool ACCharacter::IsLocallyControlledByPlayer()
{
	return GetController() && GetController()->IsLocalPlayerController();
}

void ACCharacter::BeginPlay()
{
	Super::BeginPlay();

	ConfigureOverHeadStatsWidget();

	MeshRelativeTransform = GetMesh()->GetRelativeTransform();

	PerceptionStimuliSourceComponent->RegisterForSense(UAISense_Sight::StaticClass());
}

void ACCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	if (NewController && !NewController->IsPlayerController()) //专用于AICharacter在Server上的初始化
	{
		ServerSideInit();
	}
}

void ACCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACCharacter, TeamID);
}

UAbilitySystemComponent* ACCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ACCharacter::Server_SendGameplayEventToSelf_Implementation(FGameplayTag EventTag, FGameplayEventData Payload)
{
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, EventTag, Payload);
}

bool ACCharacter::Server_SendGameplayEventToSelf_Validate(FGameplayTag EventTag, FGameplayEventData Payload)
{
	return true;
}

const TMap<ECAbilityInputID, TSubclassOf<UGameplayAbility>>& ACCharacter::GetAbilities() const
{
	return AbilitySystemComponent->GetAbilities();
}

void ACCharacter::UpgradeAbilityWithInputID(ECAbilityInputID InputID)
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->Server_UpgradeAbilityWithID(InputID);
	}
}

void ACCharacter::BindGASChangeDelegates()
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->RegisterGameplayTagEvent(CGameplayTags::Crunch_Stats_Dead).AddUObject(
			this, &ThisClass::DeathTagUpdated
		);
		AbilitySystemComponent->RegisterGameplayTagEvent(CGameplayTags::Crunch_Stats_Stun).AddUObject(
			this, &ThisClass::StunTagUpdated
		);
		AbilitySystemComponent->RegisterGameplayTagEvent(CGameplayTags::Crunch_Stats_Aim).AddUObject(
			this, &ThisClass::AimTagUpdated
		);

		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
			UCAttributeSet::GetMoveSpeedAttribute()
		).AddUObject(
			this, &ThisClass::MoveSpeedUpdated
		);

		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
			UCAttributeSet::GetMaxHealthAttribute()
		).AddUObject(
			this, &ThisClass::MaxHealthUpdated
		);

		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
			UCAttributeSet::GetMoveSpeedAttribute()
		).AddUObject(
			this, &ThisClass::MaxManaUpdated
		);
	}
}

void ACCharacter::DeathTagUpdated(FGameplayTag Tag, int NewCount)
{
	if (NewCount != 0)
	{
		StartDeathSequence();
	}
	else
	{
		Respawn();
	}
}

void ACCharacter::StunTagUpdated(FGameplayTag Tag, int NewCount)
{
	if (IsDead()) return;

	if (NewCount != 0)
	{
		OnStun();
		PlayAnimMontage(StunMontage);
	}
	else
	{
		OnRecoverFromStun();
		StopAnimMontage(StunMontage);
	}
}

void ACCharacter::AimTagUpdated(FGameplayTag Tag, int NewCount)
{
	SetIsAiming(NewCount != 0);
}

void ACCharacter::SetIsAiming(bool bIsAiming)
{
	bUseControllerRotationYaw = bIsAiming;
	GetCharacterMovement()->bOrientRotationToMovement = !bIsAiming;
	OnAimStateChanged(bIsAiming);
}

void ACCharacter::OnAimStateChanged(bool bIsAiming)
{
	// 在子类中重写该方法
}

void ACCharacter::MoveSpeedUpdated(const FOnAttributeChangeData& Data)
{
	GetCharacterMovement()->MaxWalkSpeed = Data.NewValue;
}

void ACCharacter::MaxHealthUpdated(const FOnAttributeChangeData& Data)
{
	if (IsValid(AttributeSet))
	{
		AttributeSet->RescaleHealth();
	}
}

void ACCharacter::MaxManaUpdated(const FOnAttributeChangeData& Data)
{
	if (IsValid(AttributeSet))
	{
		AttributeSet->RescaleMana();
	}
}

void ACCharacter::ConfigureOverHeadStatsWidget()
{
	if (!OverHeadWidgetComponent)
	{
		return;
	}

	if (IsLocallyControlledByPlayer())
	{
		OverHeadWidgetComponent->SetHiddenInGame(true);
		return;
	}

	// Tips: 不知道为什么UE5.5上，仅能在Client上看到Screen状态的头顶血条，listenServer端看不到
	UOverHeadStatsGauge* OverHeadStatsGauge = Cast<UOverHeadStatsGauge>(OverHeadWidgetComponent->GetUserWidgetObject());
	if (OverHeadStatsGauge)
	{
		OverHeadStatsGauge->ConfigureWithASC(GetAbilitySystemComponent());
		OverHeadWidgetComponent->SetHiddenInGame(false);
		GetWorldTimerManager().ClearTimer(HeadStatsGaugeVisibilityUpdateTimerHandle);
		GetWorldTimerManager().SetTimer(
			HeadStatsGaugeVisibilityUpdateTimerHandle, this, &ACCharacter::UpdateHeadGaugeVisibility,
			HeadStatsGaugeVisibilityCheckUpdateGap, true
		);
	}
}

void ACCharacter::UpdateHeadGaugeVisibility()
{
	APawn* LocalPlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	if (LocalPlayerPawn)
	{
		float DistSquared = FVector::DistSquared(GetActorLocation(), LocalPlayerPawn->GetActorLocation());
		OverHeadWidgetComponent->SetHiddenInGame(DistSquared > HeadStatsGaugeVisibilityRangeSquared);
	}
}

void ACCharacter::SetStatusGaugeEnabled(bool bIsEnabled)
{
	GetWorldTimerManager().ClearTimer(HeadStatsGaugeVisibilityUpdateTimerHandle);

	if (bIsEnabled)
	{
		ConfigureOverHeadStatsWidget();
	}
	else
	{
		OverHeadWidgetComponent->SetHiddenInGame(true);
	}
}

bool ACCharacter::IsDead() const
{
	return GetAbilitySystemComponent()->HasMatchingGameplayTag(CGameplayTags::Crunch_Stats_Dead);
}

void ACCharacter::RespawnImmediately()
{
	if (HasAuthority())
	{
		GetAbilitySystemComponent()->RemoveActiveEffectsWithGrantedTags(
			FGameplayTagContainer(CGameplayTags::Crunch_Stats_Dead)
		);
	}
}

void ACCharacter::DeathMontageFinished()
{
	if (IsDead()) // 防止小兵在DeathMontageFinished触发前复活而导致的bug
	{
		SetRagDollEnabled(true);
	}
}

void ACCharacter::SetRagDollEnabled(bool bIsEnabled)
{
	if (bIsEnabled)
	{
		GetMesh()->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		GetMesh()->SetSimulatePhysics(true);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	}
	else
	{
		GetMesh()->SetSimulatePhysics(false);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetMesh()->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		GetMesh()->SetRelativeTransform(MeshRelativeTransform);
	}
}

void ACCharacter::PlayDeathAnimation()
{
	if (DeathMontage)
	{
		float MontageDuration = PlayAnimMontage(DeathMontage);
		GetWorldTimerManager().SetTimer(
			DeathMontageTimerHandle,
			this,
			&ThisClass::DeathMontageFinished,
			MontageDuration + DeathMontageFinishTimeShift
		);
	}
}

void ACCharacter::StartDeathSequence()
{
	OnDeath();

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->CancelAllAbilities(); // 取消所有目前还激活着的Abilities
	}

	PlayDeathAnimation();
	SetStatusGaugeEnabled(false);
	// GetCharacterMovement()->SetMovementMode(MOVE_None); // 设置为MOVE_None便不会移动了，不适合被击飞的情况
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetAIPerceptionStimuliSourceEnabled(false); // 死了以后就不会被AI感知到
}

void ACCharacter::Respawn()
{
	OnRespawn();
	SetAIPerceptionStimuliSourceEnabled(true);
	SetRagDollEnabled(false);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	// GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	GetMesh()->GetAnimInstance()->StopAllMontages(0.f);
	SetStatusGaugeEnabled(true);

	if (HasAuthority() && GetController())
	{
		TWeakObjectPtr<AActor> StartSpot = GetController()->StartSpot;
		if (StartSpot.IsValid())
		{
			SetActorTransform(StartSpot->GetActorTransform());
		}
	}

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->ApplyFullStatsEffect();
	}
}

void ACCharacter::OnDeath()
{
}

void ACCharacter::OnRespawn()
{
}

void ACCharacter::OnStun()
{
}

void ACCharacter::OnRecoverFromStun()
{
}

void ACCharacter::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
	TeamID = NewTeamID;
}

FGenericTeamId ACCharacter::GetGenericTeamId() const
{
	return TeamID;
}

void ACCharacter::OnRep_TeamID()
{
}

void ACCharacter::SetAIPerceptionStimuliSourceEnabled(bool bIsEnabled)
{
	if (!PerceptionStimuliSourceComponent)
	{
		return;
	}

	if (bIsEnabled)
	{
		PerceptionStimuliSourceComponent->RegisterWithPerceptionSystem();
	}
	else
	{
		PerceptionStimuliSourceComponent->UnregisterFromPerceptionSystem();
	}
}

void ACCharacter::GetNetworkDebugInfo() const
{
	if (!bDebugNetworkInfo) return;

	// 1. NetMode (当前运行模式)
	FString NetModeStr;
	switch (GetNetMode())
	{
	case NM_Standalone: NetModeStr = TEXT("Standalone");
		break;
	case NM_DedicatedServer: NetModeStr = TEXT("DedicatedServer");
		break;
	case NM_ListenServer: NetModeStr = TEXT("ListenServer");
		break;
	case NM_Client: NetModeStr = TEXT("Client");
		break;
	default: NetModeStr = TEXT("Unknown");
		break;
	}

	// 2. NetRole (本端的角色)
	FString RoleStr;
	switch (GetLocalRole())
	{
	case ROLE_None: RoleStr = TEXT("None");
		break;
	case ROLE_SimulatedProxy: RoleStr = TEXT("SimulatedProxy");
		break;
	case ROLE_AutonomousProxy: RoleStr = TEXT("AutonomousProxy");
		break;
	case ROLE_Authority: RoleStr = TEXT("Authority");
		break;
	default: RoleStr = TEXT("Unknown");
		break;
	}

	// 3. RemoteRole (对端的角色)
	FString RemoteRoleStr;
	switch (GetRemoteRole())
	{
	case ROLE_None: RemoteRoleStr = TEXT("None");
		break;
	case ROLE_SimulatedProxy: RemoteRoleStr = TEXT("SimulatedProxy");
		break;
	case ROLE_AutonomousProxy: RemoteRoleStr = TEXT("AutonomousProxy");
		break;
	case ROLE_Authority: RemoteRoleStr = TEXT("Authority");
		break;
	default: RemoteRoleStr = TEXT("Unknown");
		break;
	}

	// 4. 关键判断
	bool bHasAuthority = HasAuthority();
	bool bIsLocallyControlled = IsLocallyControlled();
	bool bIsServer = (GetNetMode() == NM_DedicatedServer || GetNetMode() == NM_ListenServer);
	bool bIsClientOnly = (GetNetMode() == NM_Client);
	bool bIsStandalone = (GetNetMode() == NM_Standalone);

	// 组装信息
	const FString Info = FString::Printf(
		TEXT("=== %s ===\n")
		TEXT("NetMode: %s\n")
		TEXT("LocalRole: %s | RemoteRole: %s\n")
		TEXT("HasAuthority: %s\n")
		TEXT("IsLocallyControlled: %s\n")
		TEXT("IsServer: %s | IsClientOnly: %s | IsStandalone: %s\n")
		TEXT("Controller: %s\n")
		TEXT("PlayerState: %s"),
		*GetName(),
		*NetModeStr,
		*RoleStr,
		*RemoteRoleStr,
		bHasAuthority ? TEXT("true") : TEXT("false"),
		bIsLocallyControlled ? TEXT("true") : TEXT("false"),
		bIsServer ? TEXT("true") : TEXT("false"),
		bIsClientOnly ? TEXT("true") : TEXT("false"),
		bIsStandalone ? TEXT("true") : TEXT("false"),
		GetController() ? *GetController()->GetName() : TEXT("NULL"),
		GetPlayerState() ? *GetPlayerState()->GetPlayerName() : TEXT("NULL")
	);

	Debug::Print(Info, -1, 30.f);
}

void ACCharacter::TestPlayerPawn()
{
	// 方法1：Index 0
	APawn* PawnByIndex = UGameplayStatics::GetPlayerPawn(this, 0);

	// 方法2：找本地
	APawn* PawnByLocal = nullptr;
	for (auto It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (APlayerController* PC = It->Get())
		{
			UE_LOG(LogTemp, Warning, TEXT("PC: %s | IsLocal: %s"),
			       *PC->GetName(),
			       PC->IsLocalPlayerController() ? TEXT("Yes") : TEXT("No"));

			if (PC->IsLocalPlayerController())
			{
				PawnByLocal = PC->GetPawn();
			}
		}
	}

	Debug::Print(FString::Printf(TEXT("GetPlayerPawn(0): %s"), PawnByIndex ? *PawnByIndex->GetName() : TEXT("NULL")));
	Debug::Print(FString::Printf(TEXT("Local Pawn: %s"), PawnByLocal ? *PawnByLocal->GetName() : TEXT("NULL")));
}

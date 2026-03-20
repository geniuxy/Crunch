// Fill out your copyright notice in the Description page of Project Settings.


#include "Crunch/Public/Characters/CCharacter.h"

#include "CGameplayTags.h"
#include "CrunchDebugHelper.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerState.h"
#include "GAS/CAbilitySystemComponent.h"
#include "GAS/CAttributeSet.h"
#include "Kismet/GameplayStatics.h"
#include "Widgets/OverHeadStatsGauge.h"


ACCharacter::ACCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	AbilitySystemComponent = CreateDefaultSubobject<UCAbilitySystemComponent>(TEXT("Ability System Component"));
	AttributeSet = CreateDefaultSubobject<UCAttributeSet>(TEXT("AttributeSet"));

	OverHeadWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("Over Head Widget Component"));
	OverHeadWidgetComponent->SetupAttachment(GetRootComponent());

	BindGASChangeDelegates();
}

void ACCharacter::ServerSideInit()
{
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	AbilitySystemComponent->InitializeBaseAttributes();
	AbilitySystemComponent->GiveInitialAbilities();
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
}

void ACCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	if (NewController && !NewController->IsPlayerController()) //专用于AICharacter在Server上的初始化
	{
		ServerSideInit();
	}
}

UAbilitySystemComponent* ACCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ACCharacter::BindGASChangeDelegates()
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->RegisterGameplayTagEvent(CGameplayTags::Crunch_Stats_Dead).AddUObject(
			this, &ThisClass::DeathTagUpdated
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

void ACCharacter::PlayDeathAnimation()
{
	if (DeathMontage)
	{
		PlayAnimMontage(DeathMontage);
	}
}

void ACCharacter::StartDeathSequence()
{
	OnDeath();
	PlayDeathAnimation();
	SetStatusGaugeEnabled(false);
	GetCharacterMovement()->SetMovementMode(MOVE_None);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ACCharacter::Respawn()
{
	Debug::Print("Respawn!");
	OnRespawn();
}

void ACCharacter::OnDeath()
{
}

void ACCharacter::OnRespawn()
{
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

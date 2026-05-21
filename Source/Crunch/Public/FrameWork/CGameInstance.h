// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/IHttpRequest.h"
#include "CGameInstance.generated.h"

DECLARE_MULTICAST_DELEGATE_ThreeParams(
	FOnLoginCompleted, bool /* bWasSuccessful */, const FString& /* PlayerNickName */, const FString& /* ErrorMsg */
)
/**
 * 
 */
UCLASS()
class CRUNCH_API UCGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	void StartMatch();
	virtual void Init() override;

	/**********************************************************************/
	/*                              Login                                 */
	/**********************************************************************/
public:
	bool IsLoggedIn() const;
	bool IsLoggingIn() const;
	void ClientAccountPortalLogin();

	FOnLoginCompleted OnLoginCompleted;

private:
	void ClientLogin(const FString& Type, const FString& ID, const FString& Token);
	void LoginCompleted(int NumOfLocalPlayer, bool bWasSuccessful, const FUniqueNetId& UserID, const FString& Error);

	FDelegateHandle LoggingInDelegateHandle;

	/**********************************************************************/
	/*                Client Session Creation And Search                  */
	/**********************************************************************/
public:
	void RequestCreateAndJoinSession(const FName& NewSessionName);
	void CancelSessionCreation();

private:
	void SessionCreationRequestCompleted(
		FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully, FGuid SessionSearchID
	);
	void StartFindingCreatedSession(const FGuid& SessionSearchId);
	void StopAllSessionFindings();
	void StopFindingCreatedSession();
	void StopGlobalSessionSearch();

	FTimerHandle FindCreatedSessionTimerHandle;
	FTimerHandle FindCreatedSessionTimeoutHandle;

	UPROPERTY(EditDefaultsOnly, Category="Session Search")
	float FindCreatedSessionSearchInterval = 1.f;

	UPROPERTY(EditDefaultsOnly, Category="Session Search")
	float FindCreatedSessionTimeoutDuration = 60.f;

	void FindCreatedSession(FGuid SessionSearchID);
	void FindCreatedSessionTimeout();

	/**********************************************************************/
	/*                          Session Server                            */
	/**********************************************************************/
public:
	void PlayerJoined(const FUniqueNetIdRepl& UniqueId);
	void PlayerLeft(const FUniqueNetIdRepl& UniqueId);

private:
	void CreateSession();
	void OnSessionCreated(FName SessionName, bool bWasSuccessful);
	void EndSessionCompleted(FName SessionName, bool bWasSuccessful);

	FString ServerSessionName;
	int SessionServerPort;

	void TerminateSessionServer();

	FTimerHandle WaitPlayerJoinTimeoutHandle;

	UPROPERTY(EditDefaultsOnly, Category="Session")
	float WaitPlayerJoinTimeOutDuration = 60.f;

	void WaitPlayerJoinTimeOutReached();

	TSet<FUniqueNetIdRepl> PlayerRecord;

	/**********************************************************************/
	/*                              Level                                 */
	/**********************************************************************/
private:
	UPROPERTY(EditDefaultsOnly, Category="Map")
	TSoftObjectPtr<UWorld> MainMenuLevel;

	UPROPERTY(EditDefaultsOnly, Category="Map")
	TSoftObjectPtr<UWorld> LobbyLevel;

	UPROPERTY(EditDefaultsOnly, Category="Map")
	TSoftObjectPtr<UWorld> GameLevel;

	void LoadLevelAndListen(TSoftObjectPtr<UWorld> Level);
};

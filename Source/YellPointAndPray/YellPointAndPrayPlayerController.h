// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include <Server/MatchmakingSubsystem.h>
#include <Players/YPPCustomPlayerState.h>
#include <Items/LockPick/LockPickMiniGame.h>
#include "InputAction.h"
#include "Door.h"
#include <Server/VoiceChatComponent.h>
#include "YellPointAndPrayPlayerController.generated.h"

class UInputMappingContext;
class UUserWidget;

/**
 *  Simple first person Player Controller
 *  Manages the input mapping context.
 *  Overrides the Player Camera Manager class.
 */
UCLASS(abstract)
class YELLPOINTANDPRAY_API AYellPointAndPrayPlayerController : public APlayerController
{
	GENERATED_BODY()
	
private:
	EPlayerType StoredPlayerType = EPlayerType::None;

public:
	//VOICE CHAT
	void InitializeVoiceChat();

	UFUNCTION(Client, Reliable)
	void CloseVoiceChat();

	/** Server RPC: Request voice credentials for main channel */
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_RequestVoiceCredentials(const FString& ProductUserId);

	/** Client RPC: Receive voice credentials from server */
	UFUNCTION(Client, Reliable)
	void Client_ReceiveVoiceCredentials(const FString& ChannelName, const FString& Token);

	FString PendingChannelName;
	FString PendingToken;

	/** Constructor */
	AYellPointAndPrayPlayerController();

	void SendClickToWidget();

	void Unlock(ADoor* Door);

	UFUNCTION(Server, Reliable)
	void UnlockReal(ADoor* Door);

	ALockPickMiniGame* widget;

	AActor* User;

	UFUNCTION(Server, Reliable)
	void Server_OnRestartClicked();

	UFUNCTION(Server, Reliable)
	void Server_OnBackToMainMenuClicked();

	/** Input Mapping Contexts */
	UPROPERTY(EditAnywhere, Category="Input|Input Mappings")
	TArray<UInputMappingContext*> DrawingContexts;

	/** Input Mapping Contexts */
	UPROPERTY(EditAnywhere, Category="Input|Input Mappings")
	TArray<UInputMappingContext*> DefaultMappingContexts;

	UFUNCTION(Client, Reliable)
	void StartMinigame(TSubclassOf<AActor> MiniGameActorClass, UWorld* World, AActor* User1, ADoor* Door);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* LeftClickAction;
protected:
	//VOICE CHAT
	UVoiceChatComponent* VoiceComp;

	/** Input Mapping Contexts */
	UPROPERTY(EditAnywhere, Category="Input|Input Mappings")
	TArray<UInputMappingContext*> MobileExcludedMappingContexts;

	/** Mobile controls widget to spawn */
	UPROPERTY(EditAnywhere, Category="Input|Touch Controls")
	TSubclassOf<UUserWidget> MobileControlsWidgetClass;

	/** Pointer to the mobile controls widget */
	TObjectPtr<UUserWidget> MobileControlsWidget;

	/** Gameplay initialization */
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;

	/** Input mapping context setup */
	virtual void SetupInputComponent() override;

	virtual void Tick(float DeltaTime) override;
};

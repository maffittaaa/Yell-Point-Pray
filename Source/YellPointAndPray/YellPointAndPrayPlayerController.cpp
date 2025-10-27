// Copyright Epic Games, Inc. All Rights Reserved.


#include "YellPointAndPrayPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "YellPointAndPrayCameraManager.h"
#include "Blueprint/UserWidget.h"
#include "YellPointAndPray.h"
#include "Widgets/Input/SVirtualJoystick.h"

AYellPointAndPrayPlayerController::AYellPointAndPrayPlayerController()
{
	// set the player camera manager class
	PlayerCameraManagerClass = AYellPointAndPrayCameraManager::StaticClass();
}

void AYellPointAndPrayPlayerController::BeginPlay()
{
	Super::BeginPlay();

	
	// only spawn touch controls on local player controllers
	if (SVirtualJoystick::ShouldDisplayTouchInterface() && IsLocalPlayerController())
	{
		// spawn the mobile controls widget
		MobileControlsWidget = CreateWidget<UUserWidget>(this, MobileControlsWidgetClass);

		if (MobileControlsWidget)
		{
			// add the controls to the player screen
			MobileControlsWidget->AddToPlayerScreen(0);

		} else {

			UE_LOG(LogYellPointAndPray, Error, TEXT("Could not spawn mobile controls widget."));

		}

	}
}

void AYellPointAndPrayPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// only add IMCs for local player controllers
	if (IsLocalPlayerController())
	{
		// Add Input Mapping Context
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			for (UInputMappingContext* CurrentContext : DefaultMappingContexts)
			{
				Subsystem->AddMappingContext(CurrentContext, 0);
			}

			// only add these IMCs if we're not using mobile touch input
			if (!SVirtualJoystick::ShouldDisplayTouchInterface())
			{
				for (UInputMappingContext* CurrentContext : MobileExcludedMappingContexts)
				{
					Subsystem->AddMappingContext(CurrentContext, 0);
				}
			}
		}
	}
	
}

void AYellPointAndPrayPlayerController::OnSuccessfullyJoinedGameServer()
{
	UEOSVoiceSubsystem* VoiceSubsystem = GetGameInstance()->GetSubsystem<UEOSVoiceSubsystem>();
	if (VoiceSubsystem)
	{
		// Get the current server info from your matchmaking system
		UMatchmakingSubsystem* MatchSubsystem = GetGameInstance()->GetSubsystem<UMatchmakingSubsystem>();
		if (MatchSubsystem && MatchSubsystem->GetSessions().Num() > 0)
		{
			// Use the first session or find the current one
			FMatchSessionInfo CurrentSession = MatchSubsystem->GetSessions()[0];
			FString VoiceRoomName = FString::Printf(TEXT("Game_%s_%d"), *CurrentSession.ServerIp, CurrentSession.ServerPort);
			VoiceSubsystem->JoinVoiceRoom(VoiceRoomName);
		}
		else
		{
			// Fallback: use a simple room name
			FString VoiceRoomName = TEXT("DefaultVoiceRoom");
			VoiceSubsystem->JoinVoiceRoom(VoiceRoomName);
		}
	}
}

void AYellPointAndPrayPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Update voice proximity
	UEOSVoiceSubsystem* VoiceSubsystem = GetGameInstance()->GetSubsystem<UEOSVoiceSubsystem>();
	if (VoiceSubsystem && GetPawn())
	{
		VoiceSubsystem->SetLocalPlayerPosition(GetPawn()->GetActorLocation());
	}
}

void AYellPointAndPrayPlayerController::OnPlayerJoinedGame(const FString& PlayerId, const FVector& Position)
{
	UEOSVoiceSubsystem* VoiceSubsystem = GetGameInstance()->GetSubsystem<UEOSVoiceSubsystem>();
	if (VoiceSubsystem)
	{
		VoiceSubsystem->UpdatePlayerPosition(PlayerId, Position);
	}
}
// Copyright Epic Games, Inc. All Rights Reserved.


#include "YellPointAndPrayPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "YellPointAndPrayCameraManager.h"
#include "Blueprint/UserWidget.h"
#include "YellPointAndPray.h"
#include "Widgets/Input/SVirtualJoystick.h"
#include <Players/YPPCustomGameMode.h>
#include "EnhancedInputComponent.h"


AYellPointAndPrayPlayerController::AYellPointAndPrayPlayerController()
{
	// set the player camera manager class
	PlayerCameraManagerClass = AYellPointAndPrayCameraManager::StaticClass();
}

void AYellPointAndPrayPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	// only spawn touch controls on local player controllers
	if (IsLocalPlayerController() && SVirtualJoystick::ShouldDisplayTouchInterface())
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

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(InputComponent))
	{
		EnhancedInput->BindAction(LeftClickAction, ETriggerEvent::Started, this,&AYellPointAndPrayPlayerController::SendClickToWidget);
	}
}


void AYellPointAndPrayPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

//GAMEOVERWIDGET

void AYellPointAndPrayPlayerController::Server_OnBackToMainMenuClicked_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("Back to Main Menu in the Server"));

	AYPPCustomGameMode* GameMode = Cast<AYPPCustomGameMode>(GetWorld()->GetAuthGameMode());

	if (GameMode)
	{
		GameMode->BackToMainMenu();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("No GameMode Found"));
	}
}

void AYellPointAndPrayPlayerController::Server_OnRestartClicked_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("Restarting Level in the Server..."));

	AYPPCustomGameMode* GameMode = Cast<AYPPCustomGameMode>(GetWorld()->GetAuthGameMode());

	if (GameMode)
	{
		GameMode->RestartGame();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("No GameMode Found"));
	}
}

void AYellPointAndPrayPlayerController::StartMinigame_Implementation(TSubclassOf<AActor> MiniGameActorClass, UWorld* World, AActor* User1, ADoor* Door) {
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	widget = World->SpawnActor<ALockPickMiniGame>(MiniGameActorClass, SpawnParams);
	widget->DoorOpening = Door;
	widget->Owner = this;
	User = User1;
	Cast<AYellPointAndPrayCharacter>(User)->SetMovementInputEnabled(false);
	Cast<AYellPointAndPrayCharacter>(User)->SetLookInputEnabled(false);
	Cast<AYellPointAndPrayCharacter>(User)->SetUseActive(false);
}

void AYellPointAndPrayPlayerController::SendClickToWidget()
{
	if (widget) {
		widget->OnClick(User);
		widget = nullptr;
		User = nullptr;
	}
}

void AYellPointAndPrayPlayerController::Unlock(ADoor* Door)
{
	UnlockReal(Door);
}

void AYellPointAndPrayPlayerController::UnlockReal_Implementation(ADoor* Door)
{
	Door->UnlockDoor();
}

//LOBBY

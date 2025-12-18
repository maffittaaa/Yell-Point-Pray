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
	
	if (IsLocalPlayerController())
	{
		VoiceComp = NewObject<UVoiceChatComponent>(this);
		VoiceComp->RegisterComponent();
		VoiceComp->Initialize();

		UE_LOG(LogTemp, Warning, TEXT("[Controller] Has VoiceComp"));

		if (AYPPCustomPlayerState* CustomPlayerState = Cast<AYPPCustomPlayerState>(PlayerState)) 
		{
			UE_LOG(LogTemp, Warning, TEXT("[Controller] Has PlayerState"));
			if (CustomPlayerState->PlayerType == EPlayerType::Mute)
			{
				UE_LOG(LogTemp, Warning, TEXT("[Controller] Output Volume to 0"));
				VoiceComp->SetOutputVolume(0);
			}
			else if (CustomPlayerState->PlayerType == EPlayerType::Deaf)
			{
				UE_LOG(LogTemp, Warning, TEXT("[Controller] Input Volume to 0"));
				VoiceComp->SetInputVolume(0);
			}
		}
		else 
		{
			UE_LOG(LogTemp, Warning, TEXT("[Controller] Does not have PlayerState"));
		}
	}

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

//START VOICE CHAT IMPLEMENTATION

void AYellPointAndPrayPlayerController::InitializeVoiceChat()
{
	if (IsLocalPlayerController())
	{
		VoiceComp = NewObject<UVoiceChatComponent>(this);
		VoiceComp->RegisterComponent();
		VoiceComp->Initialize();
	}
}

void AYellPointAndPrayPlayerController::CloseVoiceChat_Implementation()
{
	if (IsLocalPlayerController())
	{
		UE_LOG(LogTemp, Log, TEXT("[PlayerController] Close Voice Chat"));
		VoiceComp->CloseVoice();
	}
}

bool AYellPointAndPrayPlayerController::Server_RequestVoiceCredentials_Validate(const FString& ProductUserId)
{
	return !ProductUserId.IsEmpty();
}

void AYellPointAndPrayPlayerController::Server_RequestVoiceCredentials_Implementation(const FString& ProductUserId)
{
	UE_LOG(LogTemp, Log, TEXT("[PlayerController] Server received voice credential request from %s"), *ProductUserId);

	// Get GameMode and request credentials
	if (AYPPCustomGameMode* GameMode = GetWorld()->GetAuthGameMode<AYPPCustomGameMode>())
	{
		GameMode->RequestVoiceCredentialsForPlayer(this, ProductUserId);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[PlayerController] GameMode not found or wrong type"));
	}
}

void AYellPointAndPrayPlayerController::Client_ReceiveVoiceCredentials_Implementation(const FString& ChannelName, const FString& Token)
{
	UE_LOG(LogTemp, Log, TEXT("[PlayerController] Client received voice credentials for channel: %s"), *ChannelName);

	// Join voice channel with server-provided credentials
	if (VoiceComp && VoiceComp->IsReady())
	{
		UE_LOG(LogTemp, Log, TEXT("[PlayerController] Voice ready, joining channel immediately"));
		VoiceComp->JoinChannel(ChannelName, Token, false);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[PlayerController] Voice component not ready, storing credentials for later"));
		// Store credentials and join when ready
		PendingChannelName = ChannelName;
		PendingToken = Token;
	}
}

//END VOICE CHAT IMPLEMENTATION

void AYellPointAndPrayPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	if (AYPPCustomPlayerState* CustomPlayerState = Cast<AYPPCustomPlayerState>(PlayerState)) 
	{
		//UE_LOG(LogTemp, Log, TEXT("[Controller] Has Player State"));
		CustomPlayerState->ReplacePlayerPawn();

		if (VoiceComp) 
		{

		}
		else 
		{
			UE_LOG(LogTemp, Error, TEXT("[Controller] Does Not Have VoiceComp"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Does not have Player State"));
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

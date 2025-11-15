// Fill out your copyright notice in the Description page of Project Settings.


#include "Server/LobbyLevelScript.h"
#include <Components/Button.h>
#include <YellPointAndPrayPlayerController.h>

void ALobbyLevelScript::OnReady()
{
	UE_LOG(LogTemp, Log, TEXT("Back to Main Menu"));
	DisableReadyWidgetButtons();

	if (LocalPlayerController)
	{
		if (AYellPointAndPrayPlayerController* PC = Cast<AYellPointAndPrayPlayerController>(LocalPlayerController))
		{
			PC->Server_OnReadyClicked();
		}
	}
}

void ALobbyLevelScript::OnCancel()
{
	UE_LOG(LogTemp, Warning, TEXT("Restarting Level..."));
	DisableReadyWidgetButtons();

	if (LocalPlayerController)
	{
		if (AYellPointAndPrayPlayerController* PC = Cast<AYellPointAndPrayPlayerController>(LocalPlayerController))
		{
			PC->Server_OnCancelClicked();
		}
	}
}
void ALobbyLevelScript::OnEnter()
{
	UE_LOG(LogTemp, Warning, TEXT("Restarting Level..."));
	DisableReadyWidgetButtons();

	if (LocalPlayerController)
	{
		if (AYellPointAndPrayPlayerController* PC = Cast<AYellPointAndPrayPlayerController>(LocalPlayerController))
		{
			PC->Server_OnEnterClicked();
		}
	}
}

void ALobbyLevelScript::DisableReadyWidgetButtons()
{
	for (UUserWidget* CurrentWidget : RegisteredPlayerWidgets)
	{
		UButton* PlayerBackToLobbyButton = Cast<UButton>(CurrentWidget->GetWidgetFromName(TEXT("BackToLobbyButton")));
		UButton* PlayerRestartLevelButton = Cast<UButton>(CurrentWidget->GetWidgetFromName(TEXT("RestartLevelButton")));

		PlayerBackToLobbyButton->SetIsEnabled(false);
		PlayerRestartLevelButton->SetIsEnabled(false);
	}
}

void ALobbyLevelScript::RegisterPlayerReadyWidget(UUserWidget* PlayerWidget, bool isHost)
{
	if (!PlayerWidget) return;

	if (RegisteredPlayerWidgets.Contains(PlayerWidget)) return;

	UE_LOG(LogTemp, Warning, TEXT("Player GameOver widget registered: %s"), *PlayerWidget->GetName());

	RegisteredPlayerWidgets.Add(PlayerWidget);

	// Bind buttons for this specific widget
	BindWidgetButtons(PlayerWidget, isHost);
}

// NEW: Bind buttons to a specific widget
void ALobbyLevelScript::BindWidgetButtons(UUserWidget* Widget, bool isHost)
{
	if (!Widget) return;

	// Find buttons in this specific widget
	UButton* PlayerReadyButton = Cast<UButton>(Widget->GetWidgetFromName(TEXT("ReadyButton")));
	UButton* PlayerCancelButton = Cast<UButton>(Widget->GetWidgetFromName(TEXT("CancelButton")));

	if (PlayerReadyButton)
	{
		PlayerReadyButton->OnClicked.AddDynamic(this, &ALobbyLevelScript::OnReady);
	}

	if (PlayerCancelButton)
	{
		PlayerCancelButton->OnClicked.AddDynamic(this, &ALobbyLevelScript::OnCancel);
	}
	
	UButton* PlayerEnterButton = Cast<UButton>(Widget->GetWidgetFromName(TEXT("EnterButton")));

	if (PlayerEnterButton)
	{
		PlayerEnterButton->OnClicked.AddDynamic(this, &ALobbyLevelScript::OnEnter);	
	}

	bHasBoundButtons = true;
}
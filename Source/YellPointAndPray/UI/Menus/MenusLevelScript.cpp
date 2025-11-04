
#include "UI/Menus/MenusLevelScript.h"
#include <Net/UnrealNetwork.h>
#include <YellPointAndPrayPlayerController.h>

void AMenusLevelScript::BeginPlay()
{
	Super::BeginPlay();
}

void AMenusLevelScript::OnBackToMainMenuClicked() 
{
	UE_LOG(LogTemp, Log, TEXT("Back to Main Menu"));
	DisableGameOverButtons();
	
	if (LocalPlayerController)
	{
		if (AYellPointAndPrayPlayerController* PC = Cast<AYellPointAndPrayPlayerController>(LocalPlayerController))
		{
			PC->Server_OnBackToMainMenuClicked();
		}
	}
}

void AMenusLevelScript::OnRestartClicked() 
{
	UE_LOG(LogTemp, Warning, TEXT("Restarting Level..."));
	DisableGameOverButtons();

	if (LocalPlayerController)
	{
		if (AYellPointAndPrayPlayerController* PC = Cast<AYellPointAndPrayPlayerController>(LocalPlayerController))
		{
			PC->Server_OnRestartClicked();
		}
	}
}

void AMenusLevelScript::DisableGameOverButtons()
{
	for (UUserWidget* CurrentWidget : RegisteredPlayerWidgets) 
	{
		UButton* PlayerBackToLobbyButton = Cast<UButton>(CurrentWidget->GetWidgetFromName(TEXT("BackToLobbyButton")));
		UButton* PlayerRestartLevelButton = Cast<UButton>(CurrentWidget->GetWidgetFromName(TEXT("RestartLevelButton")));

		PlayerBackToLobbyButton->SetIsEnabled(false);
		PlayerRestartLevelButton->SetIsEnabled(false);
	}
}

void AMenusLevelScript::RegisterPlayerGameOverWidget(UUserWidget* PlayerWidget, bool State)
{
	if (!PlayerWidget) return;

	// Change Text if Game was lost or won
	ChangeGameOverText(PlayerWidget, State);

	if (RegisteredPlayerWidgets.Contains(PlayerWidget)) return;

	UE_LOG(LogTemp, Warning, TEXT("Player GameOver widget registered: %s"), *PlayerWidget->GetName());

	RegisteredPlayerWidgets.Add(PlayerWidget);

	// Bind buttons for this specific widget
	BindWidgetButtons(PlayerWidget);
}

void AMenusLevelScript::ChangeGameOverText(UUserWidget* Widget, bool State)
{
	UTextBlock* GameOverText = Cast<UTextBlock>(Widget->GetWidgetFromName(TEXT("GameOverText")));

	if (GameOverText) 
	{
		GameOverText->SetText(State ? FText::FromString("You Win!") : FText::FromString("Game Over"));
	}
	else 
	{
		UE_LOG(LogTemp, Error, TEXT("Game Over Text was not found"));
	}
}

// NEW: Bind buttons to a specific widget
void AMenusLevelScript::BindWidgetButtons(UUserWidget* Widget)
{
	if (!Widget) return;

	// Find buttons in this specific widget
	UButton* PlayerBackToLobbyButton = Cast<UButton>(Widget->GetWidgetFromName(TEXT("BackToLobbyButton")));
	UButton* PlayerRestartLevelButton = Cast<UButton>(Widget->GetWidgetFromName(TEXT("RestartLevelButton")));

	if (PlayerBackToLobbyButton)
	{
		PlayerBackToLobbyButton->OnClicked.AddDynamic(this, &AMenusLevelScript::OnBackToMainMenuClicked);
	}

	if (PlayerRestartLevelButton)
	{
		PlayerRestartLevelButton->OnClicked.AddDynamic(this, &AMenusLevelScript::OnRestartClicked);
	}

	bHasBoundButtons = true;
}
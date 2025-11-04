// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/LevelScriptActor.h"
#include "Server/ServerButton.h"
#include <Players/YPPCustomGameMode.h>
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "MenusLevelScript.generated.h"

/**
 * 
 */
UCLASS()
class YELLPOINTANDPRAY_API AMenusLevelScript : public ALevelScriptActor
{
	GENERATED_BODY()
	
private:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UUserWidget> GameOverWidgetClass;

	UPROPERTY()
	APlayerController* LocalPlayerController;

	UFUNCTION()
	void OnRestartClicked();

	UFUNCTION()
	void OnBackToMainMenuClicked();

	UButton* BackToLobbyButton = nullptr;
	UButton* RestartLevelButton = nullptr;

	bool WidgetCreated = false;

	//virtual void Tick(float DeltaTime) override;


public:
	UUserWidget* GameOverWidget;

	UFUNCTION(BlueprintCallable, Category = "Game")
	void RegisterPlayerGameOverWidget(UUserWidget* PlayerWidget, bool State);

	void ChangeGameOverText(UUserWidget* Widget, bool State);

	// NEW: Track which players have registered their widgets
	UPROPERTY()
	TArray<UUserWidget*> RegisteredPlayerWidgets;

	bool bHasBoundButtons = false;

	// NEW: Function to bind buttons for a specific widget
	void BindWidgetButtons(UUserWidget* Widget);

	void DisableGameOverButtons();

	void SetLocalPlayerController(APlayerController* PC) { LocalPlayerController = PC; }
};

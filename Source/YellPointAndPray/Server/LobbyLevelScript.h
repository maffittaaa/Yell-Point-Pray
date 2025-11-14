// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/LevelScriptActor.h"
#include "LobbyLevelScript.generated.h"

/**
 * 
 */
UCLASS()
class YELLPOINTANDPRAY_API ALobbyLevelScript : public ALevelScriptActor
{
	GENERATED_BODY()
	
private:
	UPROPERTY()
	APlayerController* LocalPlayerController;

	bool bHasBoundButtons = false;

	UPROPERTY()
	TArray<UUserWidget*> RegisteredPlayerWidgets;
public:
	UFUNCTION()
	void OnReady();

	UFUNCTION()
	void OnCancel();

	UFUNCTION()
	void OnEnter();

	void RegisterPlayerReadyWidget(UUserWidget* PlayerWidget, bool isHost);

	void BindWidgetButtons(UUserWidget* Widget, bool isHost);

	void DisableReadyWidgetButtons();

	void SetLocalPlayerController(APlayerController* PC) { LocalPlayerController = PC; }
};

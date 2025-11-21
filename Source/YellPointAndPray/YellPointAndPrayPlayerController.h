// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include <Server/MatchmakingSubsystem.h>
#include <Players/YPPCustomPlayerState.h>
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

	/** Constructor */
	AYellPointAndPrayPlayerController();

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

protected:
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

	/** Input mapping context setup */
	virtual void SetupInputComponent() override;

	virtual void Tick(float DeltaTime) override;
};

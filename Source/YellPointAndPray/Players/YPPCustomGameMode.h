// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "YellPointAndPrayCharacter.h"
#include "GameFramework/GameModeBase.h"
#include <GameFramework/GameMode.h>
#include <GameFramework/GameSession.h>
#include "YPPCustomGameMode.generated.h"

/**
 * 
 */
UCLASS()
class YELLPOINTANDPRAY_API AYPPCustomGameMode : public AGameModeBase
{
    GENERATED_BODY()
private:
    TArray<APawn*> PlayersArray;

public:
    AYPPCustomGameMode();

    virtual void PostLogin(APlayerController* NewPlayer) override;

    void GameOver(bool State);

    void GameOverScreen(UUserWidget* PlayerWidget);

    void RestartGame();

    UPROPERTY(EditDefaultsOnly, Category = "Asymmetric")
    TSubclassOf<APawn> BlindClass;

    UPROPERTY(EditDefaultsOnly, Category = "Asymmetric")
    TSubclassOf<APawn> MuteClass;

    UPROPERTY(EditDefaultsOnly, Category = "Asymmetric")
    TSubclassOf<APawn> DeafClass;
};
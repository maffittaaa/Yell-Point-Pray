// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "YellPointAndPrayCharacter.h"
#include "Obstacles/Camera.h"
#include "YPPCustomPlayerState.h"
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
    AYPPCustomGameMode();
public:
    virtual void PostLogin(APlayerController* NewPlayer) override;
    virtual void PostSeamlessTravel() override;
    virtual void BeginPlay() override;
    void GameOver(bool State);

    void RestartGame();

    void LoadLevel(FName LevelName);
    
    void PostLoadLevel();

    void SpawnPlayer(APlayerController* NewPlayer, EPlayerType AssignedRole);

    UPROPERTY(EditDefaultsOnly, Category = "Asymmetric")
    TSubclassOf<APawn> BlindClass;

    UPROPERTY(EditDefaultsOnly, Category = "Asymmetric")
    TSubclassOf<APawn> MuteClass;

    UPROPERTY(EditDefaultsOnly, Category = "Asymmetric")
    TSubclassOf<APawn> DeafClass;
};
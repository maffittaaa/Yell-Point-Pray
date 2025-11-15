// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "YellPointAndPrayCharacter.h"
#include "Obstacles/Camera/Camera.h"
#include "YPPCustomPlayerState.h"
#include "GameFramework/GameModeBase.h"
#include <GameFramework/GameMode.h>
#include <GameFramework/GameSession.h>
#include "YPPCustomGameMode.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FPickableItemData
{
    GENERATED_BODY()

    UPROPERTY()
    TSubclassOf<APickableItem> ItemClass;

    UPROPERTY()
    FTransform Transform;

    UPROPERTY()
    FString ItemName;

    FPickableItemData(){}
    
    FPickableItemData(TSubclassOf<APickableItem> ItemClass, FTransform Transform, FString ItemName)
        : ItemClass(ItemClass), Transform(Transform), ItemName(ItemName){}
};

UCLASS()
class YELLPOINTANDPRAY_API AYPPCustomGameMode : public AGameModeBase
{
    GENERATED_BODY()
private:
    TArray<APawn*> PlayersArray;

    int PlayersReady = 0;

    AYPPCustomGameMode();
public:
    TArray<FPickableItemData> InitialItemsSpawned;
    
    void StoreAllItemsInMap();
    void RestoreAllItemsInMap();
    void DeleteAllItemsInMap();
    
    virtual void PostLogin(APlayerController* NewPlayer) override;
    void GameOver(bool State);
    
    void RestartGame();

    int GetPlayersReady();
    void PlayerGotReady();
    void PlayerCancelReady();
    void EnterGame();

    void LoadLevel(FName LevelName);

    void AddPlayerAfterChangedMap(APawn* Pawn);
    
    void SpawnPlayer(APlayerController* NewPlayer, EPlayerType AssignedRole);

    UPROPERTY(EditDefaultsOnly, Category = "Asymmetric")
    TSubclassOf<APawn> BlindClass;

    UPROPERTY(EditDefaultsOnly, Category = "Asymmetric")
    TSubclassOf<APawn> MuteClass;

    UPROPERTY(EditDefaultsOnly, Category = "Asymmetric")
    TSubclassOf<APawn> DeafClass;
};
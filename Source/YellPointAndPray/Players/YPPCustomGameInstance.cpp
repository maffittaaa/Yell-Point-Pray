// Fill out your copyright notice in the Description page of Project Settings.

#include "Players/YPPCustomGameInstance.h"
#include "YPPCustomGameMode.h"

void UYPPCustomGameInstance::Init()
{
    Super::Init();
}

void UYPPCustomGameInstance::PlayerTravelling(TSubclassOf<APawn> NewPawnClass, EPlayerType NewPlayerType, AYPPCustomPlayerState* NewPlayerState)
{
    if (!NewPawnClass) return;

    PlayerInfoArray.Add(FPlayerInfo(NewPlayerState, NewPlayerType, NewPawnClass, NewPlayerState->IsHost));
    
    DefaultClass = PlayerInfoArray[0].PawnClass;
    DefaultType = PlayerInfoArray[0].PlayerType;

    UE_LOG(LogTemp, Warning, TEXT("MI: New Player State was stored: %s"), *NewPlayerState->GetName());
    UE_LOG(LogTemp, Warning, TEXT("MI: New Player Class was stored: %s"), *NewPawnClass->GetName());
    UE_LOG(LogTemp, Warning, TEXT("MI: New Player Type was stored: %d"), NewPlayerType);
}

EPlayerType UYPPCustomGameInstance::GetPlayerType(AYPPCustomPlayerState* PlayerState)
{
    int index = GetPlayerIndex(PlayerState);
    
    if (index == -1) 
    {
        UE_LOG(LogTemp, Warning, TEXT("MI: Index is -1, Player state: %s"), *PlayerState->GetName());
        return DefaultType;
    }

    PlayerState->IsHost = PlayerInfoArray[index].bIsHost;

    if (PlayerState->IsHost)
    {
        PlayerState->LevelLoaded = true;
    }
    
    return PlayerInfoArray[index].PlayerType;
}

TSubclassOf<APawn> UYPPCustomGameInstance::GetPlayerClass(const AYPPCustomPlayerState* PlayerState)
{
    int index = GetPlayerIndex(PlayerState);

    if (index == -1)
    {
        UE_LOG(LogTemp, Warning, TEXT("MI: Index is -1"));
        return DefaultClass;
    }

    if (PlayerState->IsHost)
    {
        GoingUp = !GoingUp;
    }
    
    UE_LOG(LogTemp, Warning, TEXT("MI: Get PlayerClass: %s"), *PlayerInfoArray[index].PawnClass->GetName());
    return PlayerInfoArray[index].PawnClass;
}

void UYPPCustomGameInstance::ClearPlayerInfoArray()
{
    PlayerInfoArray.Empty();
}

int UYPPCustomGameInstance::GetPlayerIndex(const AYPPCustomPlayerState* PlayerState)
{
    FString PlayerStateName = PlayerState->GetName();
    TCHAR LastCharPlayer = PlayerStateName[PlayerStateName.Len() - 1];
    int LastNumPlayer = FCString::Atoi(&LastCharPlayer);
    
    for (int i = 0; PlayerInfoArray.Num() > i; i++)
    {
        FString ListStateName = PlayerInfoArray[i].PlayerState->GetName();
        TCHAR LastCharList = ListStateName[ListStateName.Len() - 1];

        int Players = 3;
        if (!GoingUp)
        {
            Players *= -1;
        }
        
        int LastNumList = FCString::Atoi(&LastCharList) + Players;

        if (LastNumList == LastNumPlayer)
        {
            return i;
            break;
        }
    }

    return -1;
}
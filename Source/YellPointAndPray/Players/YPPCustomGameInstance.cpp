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

	ListPlayersStates.Add(NewPlayerState);
    ListPlayersTypes.Add(NewPlayerType);
    ListPlayersPawnsClasses.Add(NewPawnClass);

    DefaultClass = ListPlayersPawnsClasses[0];
    DefaultType = ListPlayersTypes[0];

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

    UE_LOG(LogTemp, Warning, TEXT("MI: Get PlayerType: %d"), ListPlayersTypes[index]);

    return ListPlayersTypes[index];
}

TSubclassOf<APawn> UYPPCustomGameInstance::GetPlayerClass(AYPPCustomPlayerState* PlayerState)
{

    int index = GetPlayerIndex(PlayerState);

    if (index == -1)
    {
        UE_LOG(LogTemp, Warning, TEXT("MI: Index is -1"));
        return DefaultClass;
    }
    
    UE_LOG(LogTemp, Warning, TEXT("MI: Get PlayerClass: %s"), *ListPlayersPawnsClasses[index]->GetName());
    return ListPlayersPawnsClasses[index];
}

int UYPPCustomGameInstance::GetPlayerIndex(AYPPCustomPlayerState* PlayerState)
{
    FString PlayerStateName = PlayerState->GetName();
    TCHAR LastCharPlayer = PlayerStateName[PlayerStateName.Len() - 1];
    int LastNumPlayer = FCString::Atoi(&LastCharPlayer);

    if (!ListPlayersStates.IsValidIndex(0)) return -1;

    UE_LOG(LogTemp, Warning, TEXT("MI: ----- Getting Player Index -----"));
    UE_LOG(LogTemp, Warning, TEXT("MI: LastNumPlayer: %i"), LastNumPlayer);

    FString ListStateName0 = ListPlayersStates[0]->GetName();
    TCHAR LastCharList0 = ListStateName0[21];
    int LastNumList0 = FCString::Atoi(&LastCharList0) + 3;
    UE_LOG(LogTemp, Warning, TEXT("MI: 0LastNumPlayer: %s"), *ListPlayersStates[0]->GetName());
    UE_LOG(LogTemp, Warning, TEXT("MI: 0LastNumPlayer: %i"), LastNumList0);

    FString ListStateName1 = ListPlayersStates[1]->GetName();
    TCHAR LastCharList1 = ListStateName1[21];
    int LastNumList1 = FCString::Atoi(&LastCharList1) + 3;
    UE_LOG(LogTemp, Warning, TEXT("MI: 1LastNumPlayer: %s"), *ListPlayersStates[1]->GetName());
    UE_LOG(LogTemp, Warning, TEXT("MI: 1LastNumPlayer: %i"), LastNumList1);

    FString ListStateName2 = ListPlayersStates[2]->GetName();
    TCHAR LastCharList2 = ListStateName2[21];
    int LastNumList2 = FCString::Atoi(&LastCharList2) + 3;
    UE_LOG(LogTemp, Warning, TEXT("MI: 2LastNumPlayer: %s"), *ListPlayersStates[2]->GetName());
    UE_LOG(LogTemp, Warning, TEXT("MI: 2LastNumPlayer: %i"), LastNumList2);

    for (int i = 0; ListPlayersStates.Num() > i; i++)
    {
        FString ListStateName = ListPlayersStates[i]->GetName();
        TCHAR LastCharList = ListStateName[ListStateName.Len() - 1];
        int LastNumList = FCString::Atoi(&LastCharList) + 3;

        //UE_LOG(LogTemp, Warning, TEXT("MI: LastNumList: %i, LastNumPlayer: %i"), LastNumList, LastNumPlayer);

        if (LastNumList == LastNumPlayer)
        {
            UE_LOG(LogTemp, Warning, TEXT("MI: MATCH: LastNumList: %i, LastNumPlayer: %i"), LastNumList, LastNumPlayer);
            UE_LOG(LogTemp, Warning, TEXT("MI: ----- Ended Player Index -----"));
            return i;
            break;
        }
    }

    return -1;
}
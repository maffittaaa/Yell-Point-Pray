// Fill out your copyright notice in the Description page of Project Settings.

#include "Players/YPPCustomGameInstance.h"
#include "YPPCustomGameMode.h"

void UYPPCustomGameInstance::Init()
{
    Super::Init();

	PreviewImageList.Add(PreviewImageTreasure);
    PreviewImageList.Add(PreviewImageToyHammer);
    PreviewImageList.Add(PreviewImageWalkieTalkie);
    PreviewImageList.Add(PreviewImageLockPick);
    PreviewImageList.Add(PreviewImageRubberDuck);
    PreviewImageList.Add(PreviewImageFlashlight);
    PreviewImageList.Add(PreviewImageKeys);
    PreviewImageList.Add(PreviewImageKebab);
}

void UYPPCustomGameInstance::PlayerTravelling(TSubclassOf<APawn> NewPawnClass, EPlayerType NewPlayerType, AYPPCustomPlayerState* NewPlayerState, FPlayerInventoryInfo InventoryInfo)
{
    if (!NewPawnClass || !NewPlayerState) return;

    // Use the persistent Unique ID
    int32 PlayerNetId = NewPlayerState->GetPlayerId();

    // Check if the player is already in the array (e.g., if they are re-traveling) and update, otherwise add
    int32 Index = GetPlayerInfoIndexByUniqueId(PlayerNetId);
        
    if (Index == INDEX_NONE)
    {
        // Add new player info using the Unique ID
        PlayerInfoArray.Add(FPlayerInfo(PlayerNetId, NewPlayerType, NewPawnClass, NewPlayerState->IsHost, InventoryInfo));
    }
    else
    {
        // Update existing player info
        PlayerInfoArray[Index] = FPlayerInfo(PlayerNetId, NewPlayerType, NewPawnClass, NewPlayerState->IsHost, InventoryInfo);
    }

    DefaultClass = PlayerInfoArray[0].PawnClass;
    DefaultType = PlayerInfoArray[0].PlayerType;

    UE_LOG(LogTemp, Warning, TEXT("MI: FUniqueID String: %d"), NewPlayerState->GetPlayerId());
    UE_LOG(LogTemp, Warning, TEXT("MI: New Player State was stored: %s"), *NewPlayerState->GetName());
    UE_LOG(LogTemp, Warning, TEXT("MI: New Player Class was stored: %s"), *NewPawnClass->GetName());
    UE_LOG(LogTemp, Warning, TEXT("MI: New Player Type was stored: %d"), NewPlayerType);
    UE_LOG(LogTemp, Warning, TEXT("MI: Inventory slots stored: %d"), InventoryInfo.InventorySlots.Num());
}

//void UYPPCustomGameInstance::StorePlayerInventory(AYPPCustomPlayerState* PlayerState, TArray<FUInventoryStruct>& Inventory)
//{
//    int index = GetPlayerIndex(PlayerState);
//    if (index != -1)
//    {
//        PlayerInfoArray[index].InventoryInfo = FPlayerInventoryInfo(Inventory);
//        //UE_LOG(LogTemp, Warning, TEXT("MI: Stored inventory for player: %s, slots: %d"), *PlayerState->GetName(), Inventory.Num());
//    }
//}

FPlayerInventoryInfo UYPPCustomGameInstance::GetPlayerInventory(AYPPCustomPlayerState* PlayerState)
{
    int index = GetPlayerIndex(PlayerState);
    if (index != -1)
    {
        return PlayerInfoArray[index].InventoryInfo;
    }
    return FPlayerInventoryInfo();
}

EPlayerType UYPPCustomGameInstance::GetPlayerType(AYPPCustomPlayerState* PlayerState)
{
    int index = GetPlayerIndex(PlayerState);

    UE_LOG(LogTemp, Warning, TEXT("MI: GET FUniqueID String: %d"), PlayerState->GetPlayerId());

    if (index == -1) 
    {
        UE_LOG(LogTemp, Warning, TEXT("MI: Index is -1, Player state: %s"), *PlayerState->GetName());
        return DefaultType;
    }

    PlayerState->IsHost = PlayerInfoArray[index].bIsHost;
    
    return PlayerInfoArray[index].PlayerType;
}

TSubclassOf<APawn> UYPPCustomGameInstance::GetPlayerClass(AYPPCustomPlayerState* PlayerState)
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

int32 UYPPCustomGameInstance::GetPlayerInfoIndexByUniqueId(int32 UniqueId)
{
    for (int32 i = 0; i < PlayerInfoArray.Num(); ++i)
    {
        if (PlayerInfoArray[i].UniqueId != 0 && PlayerInfoArray[i].UniqueId == UniqueId)
        {
            return i;
        }
    }
    return INDEX_NONE;
}

int32 UYPPCustomGameInstance::GetPlayerIndex(AYPPCustomPlayerState* PlayerState)
{
    if (!PlayerState) return INDEX_NONE;
    return GetPlayerInfoIndexByUniqueId(PlayerState->GetPlayerId());
}
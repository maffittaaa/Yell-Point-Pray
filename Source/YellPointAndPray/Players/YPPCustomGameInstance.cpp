// Fill out your copyright notice in the Description page of Project Settings.

#include "Players/YPPCustomGameInstance.h"
#include "YPPCustomGameMode.h"

void UYPPCustomGameInstance::Init()
{
    Super::Init();

    // Bind to the post-load delegate
    if (GEngine)
    {
        FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UYPPCustomGameInstance::OnMapLoad);
    }
}

void UYPPCustomGameInstance::OnMapLoad(UWorld* World)
{
    if (!GEngine) return;

    APlayerController* PlayerController = GetFirstLocalPlayerController();

    UE_LOG(LogTemp, Warning, TEXT("MI: World %d"), World);

    if (PlayerController) 
    {
        AYPPCustomPlayerState* playerState = Cast<AYPPCustomPlayerState>(PlayerController->PlayerState);

        if (playerState) 
        {
            if (PlayerStateRef) 
            {
                playerState->LevelLoaded = true;
                playerState = PlayerStateRef;
                UE_LOG(LogTemp, Warning, TEXT("MI: Has Player"));
            }
            else 
            {
                UE_LOG(LogTemp, Warning, TEXT("MI: No Player State Ref"));
            }
        }
        else 
        {
            UE_LOG(LogTemp, Warning, TEXT("MI: Player State not found"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("MI: Player Controller not found"));
    }
}

void UYPPCustomGameInstance::PlayerLoggedIn(AYPPCustomPlayerState* NewPlayerState, EPlayerType NewPlayerType)
{
    if (!NewPlayerState) return;

    PlayerStateRef = NewPlayerState;
    PlayerType = NewPlayerType;
    UE_LOG(LogTemp, Warning, TEXT("MI: New PLayer was stored: %d"), PlayerType);

}
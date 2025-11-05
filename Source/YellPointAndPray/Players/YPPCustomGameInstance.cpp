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
    UE_LOG(LogTemp, Warning, TEXT("GI: OnMapLoad called - World: %s, NetMode: %d, HasAuthGameMode: %s"),
        *GetNameSafe(World),
        World ? World->GetNetMode() : -1,
        World && World->GetAuthGameMode() ? TEXT("Yes") : TEXT("No"));
}

void UYPPCustomGameInstance::PlayerLoggedIn(AYPPCustomPlayerState* NewPlayerState, EPlayerType NewPlayerType)
{
    if (!NewPlayerState) return;

    FLoggedInPlayer NewPlayer;
    NewPlayer.PlayerStateRef = NewPlayerState;
    NewPlayer.PlayerType = NewPlayerType;

    PlayersLoggedIn.Add(NewPlayer);
}

bool UYPPCustomGameInstance::HasPlayers() 
{
    if (PlayersLoggedIn.Num() <= 0) return false;

    for (FLoggedInPlayer CurrentPlayerSocket : PlayersLoggedIn)
    {
        if (CurrentPlayerSocket.PlayerStateRef == nullptr)
        {
            return false;
        }
    }

    return true;
}

EPlayerType UYPPCustomGameInstance::GetPlayer(const AYPPCustomPlayerState* PlayerState)
{
    if (!PlayerState) return EPlayerType::None;

    for (const FLoggedInPlayer& CurrentPlayer : PlayersLoggedIn)
    {
        if (CurrentPlayer.PlayerStateRef == PlayerState)
        {
            return CurrentPlayer.PlayerType;
        }
    }
    return EPlayerType::None;
}
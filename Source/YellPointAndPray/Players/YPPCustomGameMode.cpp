// Fill out your copyright notice in the Description page of Project Settings.


#include "YPPCustomGameMode.h"
#include "YPPCustomPlayerState.h"
#include "GameFramework/GameStateBase.h"

AYPPCustomGameMode::AYPPCustomGameMode()
{
    PlayerStateClass = AYPPCustomPlayerState::StaticClass();

    DefaultPawnClass = nullptr;
}

void AYPPCustomGameMode::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);

    if (!HasAuthority() || !NewPlayer) return;

    // Determine role based on current players on GameState (server authoritative)
    int32 NumPlayers = 0;

    if (GetWorld() && GetWorld()->GetGameState()) 
    {
        NumPlayers = GetWorld()->GetGameState()->PlayerArray.Num(); // includes the newly added PlayerState
    }

    AYPPCustomPlayerState* PlayerState = NewPlayer->GetPlayerState<AYPPCustomPlayerState>();
    if (!PlayerState) return;

    EPlayerType AssignedRole = EPlayerType::None;

    if (NumPlayers == 1)
    {
        AssignedRole = EPlayerType::Blind;
    }
    else if (NumPlayers == 2)
    {
        AssignedRole = EPlayerType::Mute;
    }
    else if (NumPlayers == 3)
    {
        AssignedRole = EPlayerType::Deaf;
    }

    PlayerState->SetPlayerType(AssignedRole);
    // Choose Pawn class
    TSubclassOf<APawn> PawnClassToSpawn = nullptr;

    if (AssignedRole == EPlayerType::Blind) PawnClassToSpawn = BlindClass;
    else if (AssignedRole == EPlayerType::Mute) PawnClassToSpawn = MuteClass;
    else if (AssignedRole == EPlayerType::Deaf) PawnClassToSpawn = DeafClass;

    // If we have a pawn class, spawn and possess it
    if (PawnClassToSpawn)
    {
        AActor* Start = ChoosePlayerStart(NewPlayer); // you can override ChoosePlayerStart if you want role-specific starts
        FTransform SpawnTransform = Start ? Start->GetActorTransform() : FTransform::Identity;

        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = NewPlayer;
        APawn* NewPawn = GetWorld()->SpawnActor<APawn>(PawnClassToSpawn, SpawnTransform, SpawnParams);
        if (NewPawn)
        {
            NewPlayer->Possess(NewPawn);
        }
    }
    else
    {
        // Fallback: call default restart behavior
        RestartPlayer(NewPlayer);
    }
}
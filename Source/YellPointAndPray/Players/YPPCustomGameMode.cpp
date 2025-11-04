// Fill out your copyright notice in the Description page of Project Settings.


#include "YPPCustomGameMode.h"
#include "YPPCustomPlayerState.h"
#include "YPPBlindCharacter.h"
#include "YPPMuteCharacter.h"
#include "YPPDeafCharacter.h"
#include <GameFramework/GameMode.h>
#include "GameFramework/GameStateBase.h"
#include <UI/Menus/MenusLevelScript.h>
#include <EnhancedInputSubsystems.h>
#include <Kismet/GameplayStatics.h>
#include <YellPointAndPrayPlayerController.h>

AYPPCustomGameMode::AYPPCustomGameMode()
{
    PlayerStateClass = AYPPCustomPlayerState::StaticClass();
    //GetWorld()->GetNetDriver()->SetNetDriverName(NAME_GameNetDriver);
    DefaultPawnClass = nullptr;
    bUseSeamlessTravel = true;
}

void AYPPCustomGameMode::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);

    if (!HasAuthority() || !NewPlayer) return;

    AYPPCustomPlayerState* PlayerState = NewPlayer->GetPlayerState<AYPPCustomPlayerState>();
    if (!PlayerState) return;

    bool bHasBlind = false;
    bool bHasDeaf = false;
    bool bHasMute = false;

    for (APlayerState* CurrentPlayerState : GetWorld()->GetGameState()->PlayerArray)
    {
        if (CurrentPlayerState == PlayerState) continue;

        if (AYPPCustomPlayerState* YPPPlayerState = Cast<AYPPCustomPlayerState>(CurrentPlayerState))
        {
            switch (YPPPlayerState->PlayerType)
            {
            case EPlayerType::Blind:
                bHasBlind = true;
                break;
            case EPlayerType::Deaf:
                bHasDeaf = true;
                break; 
            case EPlayerType::Mute:
                bHasMute = true;
                break;
            }
        }
    }

    EPlayerType AssignedRole = EPlayerType::None;

    if (!bHasBlind)
    {
        AssignedRole = EPlayerType::Blind;
    }
    else if (!bHasDeaf)
    {
        AssignedRole = EPlayerType::Deaf;
    }
    else if (!bHasMute)
    {
        AssignedRole = EPlayerType::Mute;
    }

    PlayerState->SetPlayerType(AssignedRole);

    if (GetWorld()->GetGameState()->PlayerArray.Num() == 1)
    {
        PlayerState->IsHost = true;
    }

    TSubclassOf<APawn> PawnClassToSpawn = nullptr;

    if (AssignedRole == EPlayerType::Blind)
        PawnClassToSpawn = BlindClass;
    else if (AssignedRole == EPlayerType::Deaf)
        PawnClassToSpawn = DeafClass;
    else if (AssignedRole == EPlayerType::Mute)
        PawnClassToSpawn = MuteClass;


    if (PawnClassToSpawn)
    {
        FTimerHandle TimerHandle;
        GetWorldTimerManager().SetTimer(TimerHandle, [this, NewPlayer, PawnClassToSpawn, AssignedRole]()
            {
                if (NewPlayer && NewPlayer->GetPawn() == nullptr)
                {
                    AActor* Start = ChoosePlayerStart(NewPlayer);
                    FTransform SpawnTransform = Start ? Start->GetActorTransform() : FTransform::Identity;

                    FActorSpawnParameters SpawnParams;
                    SpawnParams.Owner = NewPlayer;

                    APawn* NewPawn = GetWorld()->SpawnActor<APawn>(PawnClassToSpawn, SpawnTransform, SpawnParams);

                    PlayersArray.Add(NewPawn);
                    
                    if (NewPawn)
                    {
                        NewPlayer->Possess(NewPawn);
                    }
                }
            }, 0.5f, false);
    }
    else
    {
        RestartPlayer(NewPlayer);
    }
}

void AYPPCustomGameMode::GameOver(bool State)
{
    UE_LOG(LogTemp, Warning, TEXT("Game Over"));

    for (APawn* Player : PlayersArray)
    {
        AYellPointAndPrayCharacter* CurrentPlayer = Cast<AYellPointAndPrayCharacter>(Player);
        if (CurrentPlayer)
        {
            CurrentPlayer->Client_ShowGameOver(State);
        }
    }
}

void AYPPCustomGameMode::RestartGame()
{
    if (!HasAuthority())
    {
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("Restarstiiing"));

    TArray<AActor*> Items;
    TArray<AActor*> Guards;
    TArray<AActor*> Cameras;

    UGameplayStatics::GetAllActorsOfClass(GetWorld(), APickableItem::StaticClass(), Items);
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGuard::StaticClass(), Guards);
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACamera::StaticClass(), Cameras);

    for (auto& Item : Items)
    {
        APickableItem* CurrentItem = Cast<APickableItem>(Item);
        if (CurrentItem)
        {
            //CurrentItem->Destroy();
        }
    }

    for (auto& Guard : Guards)
    {
        AGuard* CurrentGuard = Cast<AGuard>(Guard);
        if (CurrentGuard)
        {
            //CurrentGuard->Destroy();
        }
    }

    for (auto& Camera : Cameras)
    {
        ACamera* CurrentCamera = Cast<ACamera>(Camera);
        if (CurrentCamera) 
        {
			CurrentCamera->ResetCameraObstacle();
        }
    }

    for (APawn* Player : PlayersArray)
    {
        AYellPointAndPrayCharacter* CurrentPlayer = Cast<AYellPointAndPrayCharacter>(Player);
        if (CurrentPlayer)
        {
            UE_LOG(LogTemp, Warning, TEXT("Has Player"));
            AYellPointAndPrayPlayerController* PC = Cast<AYellPointAndPrayPlayerController>(CurrentPlayer->GetController());
            if (PC)
            {
                UE_LOG(LogTemp, Warning, TEXT("Has Player Controller"));
                CurrentPlayer->Client_HideGameOver();
            }
        }
    }
}

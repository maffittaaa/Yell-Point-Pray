// Fill out your copyright notice in the Description page of Project Settings.


#include "YPPCustomGameMode.h"
#include "YPPBlindCharacter.h"
#include "YPPMuteCharacter.h"
#include "YPPDeafCharacter.h"
#include <GameFramework/GameMode.h>
#include "GameFramework/GameStateBase.h"
#include <UI/Menus/MenusLevelScript.h>
#include <EnhancedInputSubsystems.h>
#include <Kismet/GameplayStatics.h>
#include <YellPointAndPrayPlayerController.h>
#include "YPPCustomGameInstance.h"

AYPPCustomGameMode::AYPPCustomGameMode()
{
    PlayerStateClass = AYPPCustomPlayerState::StaticClass();
    //GetWorld()->GetNetDriver()->SetNetDriverName(NAME_GameNetDriver);
    //DefaultPawnClass = AYPPBlindCharacter::StaticClass();
    bUseSeamlessTravel = true;
    bPauseable = false;
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

    UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(GetWorld());

    UYPPCustomGameInstance* YPPGameInstance = Cast<UYPPCustomGameInstance>(GameInstance);

    if (YPPGameInstance)
    {
        YPPGameInstance->PlayerLoggedIn(PlayerState, AssignedRole);
    }

    SpawnPlayer(NewPlayer, AssignedRole);
}

void AYPPCustomGameMode::SpawnPlayer(APlayerController* NewPlayer, EPlayerType AssignedRole)
{
    if (!NewPlayer || !GetWorld())
    {
        UE_LOG(LogTemp, Error, TEXT("GI: SpawnPlayer: Invalid NewPlayer or World"));
        return;
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
        UE_LOG(LogTemp, Warning, TEXT("GI: Has pawn class"));

        if (NewPlayer)
        {
            AActor* Start = ChoosePlayerStart(NewPlayer);
            FTransform SpawnTransform = Start ? Start->GetActorTransform() : FTransform::Identity;

            FActorSpawnParameters SpawnParams;
            SpawnParams.Owner = NewPlayer;

            APawn* NewPawn = GetWorld()->SpawnActor<APawn>(PawnClassToSpawn, SpawnTransform, SpawnParams);

            PlayersArray.Add(NewPawn);

            if (NewPawn)
            {
                if (NewPlayer->GetPawn())
                {
                    NewPlayer->GetPawn()->Destroy();
                }
                UE_LOG(LogTemp, Warning, TEXT("GI: Pawn Possessed"));

                NewPlayer->Possess(NewPawn);
            }
        }
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

    TArray<AActor*> Resetables;

    // Get all actors that implement the UReset interface
    UGameplayStatics::GetAllActorsWithInterface(GetWorld(), UReset::StaticClass(), Resetables);

    for (auto& Actor : Resetables)
    {
        IReset::Execute_Reset(Actor);
    }
}

void AYPPCustomGameMode::LoadLevel(FName LevelName) 
{
    if (GetWorld()->GetNetMode() == NM_ListenServer || GetWorld()->GetNetMode() == NM_DedicatedServer)
    {
        // Server tells all clients to travel
        for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
        {
            if (APlayerController* PC = It->Get())
            {
                //PC->ClientTravel("/Game/FirstPerson/Lvl_MainTest", TRAVEL_Relative);
            }
        }
    }
    GetWorld()->ServerTravel(TEXT("/Game/FirstPerson/Lvl_MainTest?listen"));
}

void AYPPCustomGameMode::BeginPlay()
{
    Super::BeginPlay();

    // Only process on server
    if (HasAuthority())
    {
        // Small delay to ensure all players have connected after seamless travel
        FTimerHandle TimerHandle;
        GetWorldTimerManager().SetTimer(TimerHandle, [this]()
            {
                UE_LOG(LogTemp, Warning, TEXT("GM: BeginPlay - Respawning Players in New Level"));
                PostLoadLevel();
            }, 1.0f, false); // 1 second delay to ensure all players are ready
    }
}

void AYPPCustomGameMode::PostSeamlessTravel()
{
    Super::PostSeamlessTravel();

    // Just log - we'll use BeginPlay for spawning in the new level
    UE_LOG(LogTemp, Warning, TEXT("GM: PostSeamlessTravel called (using BeginPlay for spawning)"));
}

void AYPPCustomGameMode::PostLoadLevel()
{
    if (!HasAuthority() || !GetWorld() || !GameState)
    {
        UE_LOG(LogTemp, Error, TEXT("GM: PostLoadLevel - Missing authority, world, or gamestate"));
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("GM: PostLoadLevel - Respawning Players via GameState"));

    PlayersArray.Empty();

    // Use GameState PlayerArray which is more stable than PlayerController iterator
    UE_LOG(LogTemp, Warning, TEXT("GM: PlayerArray count: %d"), GameState->PlayerArray.Num());

    UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(GetWorld());
    UYPPCustomGameInstance* YPPGameInstance = Cast<UYPPCustomGameInstance>(GameInstance);

    if (YPPGameInstance)
    {
        for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
        {
            if (APlayerController* PC = It->Get())
            {
                AYPPCustomPlayerState* PlayerState = PC->GetPlayerState<AYPPCustomPlayerState>();

                EPlayerType AssignedRole = YPPGameInstance->GetPlayer(PlayerState);

                UE_LOG(LogTemp, Warning, TEXT("GM: Successfully got GameInstance %d"), AssignedRole);

                SpawnPlayer(PC, AssignedRole);
                // Use your custom game instance
                UE_LOG(LogTemp, Warning, TEXT("GM: Successfully got GameInstance"));
            }
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("GM: Failed to get GameInstance or wrong type"));
    }
}
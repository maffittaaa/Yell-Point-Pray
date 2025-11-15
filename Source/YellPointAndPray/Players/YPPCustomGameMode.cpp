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

void AYPPCustomGameMode::AddPlayerAfterChangedMap(APawn* Pawn)
{
    PlayersArray.Add(Pawn);
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

void AYPPCustomGameMode::StoreAllItemsInMap()
{
    TSubclassOf<APickableItem> ClassToFind = APickableItem::StaticClass();
    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ClassToFind, FoundActors);

    InitialItemsSpawned.Empty();

    for (AActor* Actor : FoundActors)
    {
        APickableItem* PickableItem = Cast<APickableItem>(Actor);
        if (PickableItem && PickableItem->IsValidLowLevel())
        {
            FPickableItemData ItemData = FPickableItemData(PickableItem->GetClass(), PickableItem->GetActorTransform(), PickableItem->GetName());
            
            InitialItemsSpawned.Add(ItemData);
            
            UE_LOG(LogTemp, Warning, TEXT("Stored PickableItem: %s at location %s"), 
                *PickableItem->GetName(), 
                *ItemData.Transform.GetLocation().ToString());
        }
    }
}

void AYPPCustomGameMode::DeleteAllItemsInMap()
{
    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), APickableItem::StaticClass(), FoundActors);

    for (AActor* Actor : FoundActors)
    {
        Actor->Destroy();
    }
}

void AYPPCustomGameMode::RestoreAllItemsInMap()
{
    if (GetWorld() == nullptr) return;

    DeleteAllItemsInMap();
    
    for (const FPickableItemData& ItemData : InitialItemsSpawned)
    {
        if (ItemData.ItemClass && ItemData.ItemClass->IsValidLowLevel())
        {
            FActorSpawnParameters SpawnParams;
            
            APickableItem* NewItem = GetWorld()->SpawnActor<APickableItem>(
                ItemData.ItemClass, 
                ItemData.Transform, 
                SpawnParams
            );
            
            if (NewItem)
            {
                UE_LOG(LogTemp, Warning, TEXT("Restored PickableItem: %s"), *ItemData.ItemName);
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("Failed to restore PickableItem: %s"), *ItemData.ItemName);
            }
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

    RestoreAllItemsInMap();

    for (auto& Actor : Resetables)
    {
        IReset::Execute_Reset(Actor);
    }
}

void AYPPCustomGameMode::PlayerGotReady() 
{
    PlayersReady++;
}

void AYPPCustomGameMode::PlayerCancelReady() 
{
    PlayersReady--;
}

int AYPPCustomGameMode::GetPlayersReady()
{
    return PlayersReady;
}

void AYPPCustomGameMode::EnterGame() 
{
    LoadLevel("Lvl_MainTest");
}

void AYPPCustomGameMode::LoadLevel(FName LevelName) 
{
    if (GetWorld()->GetNetMode() == NM_ListenServer || GetWorld()->GetNetMode() == NM_DedicatedServer)
    {
        if (UGameInstance* GameInstance = GetWorld()->GetGameInstance())
        {
            if (UYPPCustomGameInstance* CustomGameInstance = Cast<UYPPCustomGameInstance>(GameInstance))
            {
                // CustomGameInstance->ClearPlayerInfoArray();
                
                for (APawn* Pawn : PlayersArray)
                {
                    AController* Controller = Pawn->GetController();
            
                    if (APlayerController* PC = Cast<APlayerController>(Controller))
                    {
                        if (AYellPointAndPrayPlayerController* YPPPlayerController = Cast<AYellPointAndPrayPlayerController>(PC)) 
                        {
                            if (AYPPCustomPlayerState* PlayerState = Cast<AYPPCustomPlayerState>(PC->PlayerState)) 
                            {
                                if (AYellPointAndPrayCharacter* PlayerCharacter = Cast<AYellPointAndPrayCharacter>(PC->GetPawn()))
                                {
                                    // Store the current inventory for travel
                                    TArray<FUInventoryStruct> CurrentInventory = PlayerCharacter->InventoryComponent->GetAllInventory();

                                    // Store in GameInstance
                                    CustomGameInstance->StorePlayerInventory(PlayerState, CurrentInventory);

                                    UE_LOG(LogTemp, Warning, TEXT("Travelling: Character: %s"), *PlayerCharacter->GetName());
                                    UE_LOG(LogTemp, Warning, TEXT("Travelling: PlayerType: %d"), PlayerState->PlayerType);

                                    for (int i = 0; i < CurrentInventory.Num(); i++) 
                                    {
                                        UE_LOG(LogTemp, Warning, TEXT("Travelling: Inventory slot %i: %s"), i, *CurrentInventory[i].Name);
                                    }

                                    CustomGameInstance->PlayerTravelling(YPPPlayerController->GetPawn()->GetClass(), PlayerState->PlayerType, PlayerState, CurrentInventory);
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    PlayersArray.Empty();
    GetWorld()->ServerTravel(TEXT("/Game/FirstPerson/Lvl_MainTest?listen"));
}
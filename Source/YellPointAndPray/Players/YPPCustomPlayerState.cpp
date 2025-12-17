// Fill out your copyright notice in the Description page of Project Settings.


#include "YPPCustomPlayerState.h"
#include "Net/UnrealNetwork.h"
#include <Kismet/GameplayStatics.h>
#include "YPPCustomGameMode.h"
#include "Players/YPPCustomGameMode.h"
#include "YPPCustomGameInstance.h"

AYPPCustomPlayerState::AYPPCustomPlayerState()
{
	// PlayerState already replicates by default on server
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	bReplicates = true;
}

void AYPPCustomPlayerState::BeginPlay()
{
	Super::BeginPlay();

	if (!HasAuthority()) return;
}

void AYPPCustomPlayerState::ReplacePlayerPawn()
{
	UYPPCustomGameInstance* CustomGameInstance = Cast<UYPPCustomGameInstance>(GetWorld()->GetGameInstance());

	if (!CustomGameInstance) 
	{
		UE_LOG(LogTemp, Warning, TEXT("PS: CustomGameInstance is null"));
		return;
	} 
    
	if (PlayerType != EPlayerType::None) 
	{
		UE_LOG(LogTemp, Warning, TEXT("PS: Player Type already exists, not creating again")); 
		return;
	}
	else 
	{
		UE_LOG(LogTemp, Warning, TEXT("PS: Player Type does not exist, creating..."));
	}

	if (CustomGameInstance->GetPlayerType(this) != EPlayerType::None)
	{
		AController* PlayerController = GetPlayerController();

		if (!PlayerController) 
		{
			UE_LOG(LogTemp, Warning, TEXT("PS: PlayerController is null, PlayerType: %d"), CustomGameInstance->GetPlayerType(this));
			return;
		}

		PlayerType = CustomGameInstance->GetPlayerType(this);
        
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = PlayerController;

		AYPPCustomGameMode* CustomGameMode = Cast<AYPPCustomGameMode>(GetWorld()->GetAuthGameMode());

		AActor* Start = CustomGameMode->ChoosePlayerStart(PlayerController);

		FTransform SpawnTransform = Start ? Start->GetActorTransform() : FTransform::Identity;
        
		APawn* NewPawn = GetWorld()->SpawnActor<APawn>(
			CustomGameInstance->GetPlayerClass(this), 
			SpawnTransform,
			SpawnParams
		);
        
		if (NewPawn) 
		{
			if (PlayerController->GetPawn())
			{
				PlayerController->GetPawn()->Destroy();
				UE_LOG(LogTemp, Warning, TEXT("PS: Destroying Current Pawn"));
			}

			PlayerController->Possess(NewPawn);
			Cast<AYPPCustomGameMode>(GetWorld()->GetAuthGameMode())->AddPlayerAfterChangedMap(NewPawn);
			
			AYellPointAndPrayCharacter* Character = Cast<AYellPointAndPrayCharacter>(NewPawn);

			if (Character) 
			{
				Character->RestoreTravelInventory();
			}

			UE_LOG(LogTemp, Warning, TEXT("PS: Has Player Possessing now, PlayerType: %d"), CustomGameInstance->GetPlayerType(this));
		}
		else 
		{
			UE_LOG(LogTemp, Warning, TEXT("PS: Pawn is null"));
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("PS: IsHost: %d"), IsHost);
}

void AYPPCustomPlayerState::OnRep_PlayerType()
{
	// Client-side reaction to role change (update HUD / visuals, etc.)
}

void AYPPCustomPlayerState::SetPlayerType(EPlayerType NewType)
{
	if (HasAuthority())
	{
		PlayerType = NewType;
		// If you need immediate client-side handling on server, you can call OnRep manually:
		// OnRep_PlayerRole();
	}
}

void AYPPCustomPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AYPPCustomPlayerState, PlayerType);
	DOREPLIFETIME(AYPPCustomPlayerState, IsHost);
	DOREPLIFETIME(AYPPCustomPlayerState, bIsReady);
}

void AYPPCustomPlayerState::OnRep_IsPlayerReady()
{
	// This will be called on all clients when bIsReady changes.
	// Here, you can add visual or audio feedback for the state change.
	UE_LOG(LogTemp, Warning, TEXT("Player's ready state is now: %s"), bIsReady ? TEXT("True") : TEXT("False"));
}
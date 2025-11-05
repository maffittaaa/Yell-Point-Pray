// Fill out your copyright notice in the Description page of Project Settings.


#include "YPPCustomPlayerState.h"
#include "Net/UnrealNetwork.h"
#include <Kismet/GameplayStatics.h>
#include "YPPCustomGameMode.h"

AYPPCustomPlayerState::AYPPCustomPlayerState()
{
	// PlayerState already replicates by default on server
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	bReplicates = true;
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
	DOREPLIFETIME(AYPPCustomPlayerState, LevelLoaded);
}

void AYPPCustomPlayerState::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!HasAuthority()) return;

	TimePassed -= DeltaTime;
	//UE_LOG(LogTemp, Warning, TEXT("Time passed: %f"), TimePassed);
	if (TimePassed > 1000000 && !LevelLoaded)
	{
		LevelLoaded = true;
		FName LevelName = "Lvl_MainTest";
		ChangeToLevel(LevelName);
	}
}


void AYPPCustomPlayerState::ChangeToLevel(FName LevelName)
{
	UE_LOG(LogTemp, Warning, TEXT("Game is Starting"));

	AYPPCustomGameMode* GameMode = Cast<AYPPCustomGameMode>(GetWorld()->GetAuthGameMode());
	if (GameMode)
	{
		GameMode->LoadLevel(LevelName);
	}
}
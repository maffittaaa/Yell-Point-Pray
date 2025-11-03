// Fill out your copyright notice in the Description page of Project Settings.


#include "YPPCustomPlayerState.h"
#include "Net/UnrealNetwork.h"

AYPPCustomPlayerState::AYPPCustomPlayerState()
{
	// PlayerState already replicates by default on server
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
}
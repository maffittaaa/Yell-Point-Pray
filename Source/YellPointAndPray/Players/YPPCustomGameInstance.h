// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Engine/Engine.h"
#include "YPPCustomPlayerState.h"
#include "GameFramework/PlayerState.h"
#include "YPPCustomGameInstance.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct FLoggedInPlayer
{
	GENERATED_BODY()

public:
	UPROPERTY()
	// Storing a reference to the unique object associated with the player
	AYPPCustomPlayerState* PlayerStateRef = nullptr;

	UPROPERTY()
	EPlayerType PlayerType = EPlayerType::None;
};

UCLASS()
class YELLPOINTANDPRAY_API UYPPCustomGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	void PlayerLoggedIn(AYPPCustomPlayerState* NewPlayerState, EPlayerType NewPlayerType);
	bool HasPlayers();
	EPlayerType GetPlayer(const AYPPCustomPlayerState* PlayerState);

	UPROPERTY()
	TArray<FLoggedInPlayer> PlayersLoggedIn;

protected:
	virtual void Init() override;

private:
	void OnMapLoad(UWorld* World);

	// FIX: Use FName to track the map, as it's a stable identifier across delegate calls.
	UPROPERTY()
	FName LastLoadedMapName = FName(TEXT("None")); // <--- ADDED THIS LINE

};

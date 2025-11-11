// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Engine/Engine.h"
#include "list"
#include "YPPCustomPlayerState.h"
#include "GameFramework/PlayerState.h"
#include "YPPCustomGameInstance.generated.h"

/**
 * 
 */

UCLASS()
class YELLPOINTANDPRAY_API UYPPCustomGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	void PlayerTravelling(TSubclassOf<APawn> NewPawnClass, EPlayerType NewPlayerType, AYPPCustomPlayerState* PlayerState);

	UPROPERTY()
	AYPPCustomPlayerState* PlayerStateRef = nullptr;
	
	EPlayerType GetPlayerType(AYPPCustomPlayerState* PlayerState);

	TSubclassOf<APawn> GetPlayerClass(AYPPCustomPlayerState* PlayerState);

	TArray<AYPPCustomPlayerState*> ListPlayersStates;
	TArray<EPlayerType> ListPlayersTypes;
	TArray<TSubclassOf<APawn>> ListPlayersPawnsClasses;
protected:

	UPROPERTY()
	EPlayerType DefaultType = EPlayerType::None;

	UPROPERTY(EditDefaultsOnly, Category = "Asymmetric")
	TSubclassOf<APawn> DefaultClass;

	int GetPlayerIndex(AYPPCustomPlayerState* PlayerState);

	virtual void Init() override;
};

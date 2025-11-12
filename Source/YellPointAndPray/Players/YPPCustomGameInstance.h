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

USTRUCT(BlueprintType)
struct FPlayerInfo
{
	GENERATED_BODY()

	UPROPERTY()
	AYPPCustomPlayerState* PlayerState = nullptr;

	UPROPERTY()
	EPlayerType PlayerType = EPlayerType::None;

	UPROPERTY()
	TSubclassOf<APawn> PawnClass = nullptr;

	UPROPERTY()
	bool bIsHost = false;

	// Constructor for easy initialization
	FPlayerInfo() {}
    
	FPlayerInfo(AYPPCustomPlayerState* InPlayerState, EPlayerType InPlayerType, TSubclassOf<APawn> InPawnClass, bool bInIsHost)
		: PlayerState(InPlayerState), PlayerType(InPlayerType), PawnClass(InPawnClass), bIsHost(bInIsHost) {}
};

UCLASS()
class YELLPOINTANDPRAY_API UYPPCustomGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	void PlayerTravelling(TSubclassOf<APawn> NewPawnClass, EPlayerType NewPlayerType, AYPPCustomPlayerState* PlayerState);

	UPROPERTY()
	AYPPCustomPlayerState* PlayerStateRef = nullptr;
	
	EPlayerType GetPlayerType(AYPPCustomPlayerState* PlayerState);

	TSubclassOf<APawn> GetPlayerClass(const AYPPCustomPlayerState* PlayerState);

	void ClearPlayerInfoArray();
	
	TArray<FPlayerInfo> PlayerInfoArray;
protected:

	UPROPERTY()
	EPlayerType DefaultType = EPlayerType::None;

	UPROPERTY(EditDefaultsOnly, Category = "Asymmetric")
	TSubclassOf<APawn> DefaultClass;

	bool GoingUp = true;
	
	int GetPlayerIndex(const AYPPCustomPlayerState* PlayerState);

	virtual void Init() override;
};

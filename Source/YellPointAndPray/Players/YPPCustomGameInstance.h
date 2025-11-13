// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Engine/Engine.h"
#include "list"
#include "UI/Inventory/Inventory.h"
#include "YPPCustomPlayerState.h"
#include "GameFramework/PlayerState.h"
#include "YPPCustomGameInstance.generated.h"

/**
 * 
 */

USTRUCT()
struct FPlayerInventoryInfo
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FUInventoryStruct> InventorySlots;

	FPlayerInventoryInfo() {}
	FPlayerInventoryInfo(const TArray<FUInventoryStruct>& Slots) : InventorySlots(Slots) {}
};

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

	UPROPERTY() // Add this
	FPlayerInventoryInfo InventoryInfo;

	FPlayerInfo() : PlayerState(nullptr), PlayerType(EPlayerType::None), PawnClass(nullptr), bIsHost(false) {}
	FPlayerInfo(AYPPCustomPlayerState* InPlayerState, EPlayerType InPlayerType, TSubclassOf<APawn> InPawnClass, bool InIsHost, const FPlayerInventoryInfo& InInventoryInfo)
		: PlayerState(InPlayerState), PlayerType(InPlayerType), PawnClass(InPawnClass), bIsHost(InIsHost), InventoryInfo(InInventoryInfo) {
	}
};

UCLASS()
class YELLPOINTANDPRAY_API UYPPCustomGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	void PlayerTravelling(TSubclassOf<APawn> NewPawnClass, EPlayerType NewPlayerType, AYPPCustomPlayerState* PlayerState, const FPlayerInventoryInfo& InventoryInfo);

	void StorePlayerInventory(AYPPCustomPlayerState* PlayerState, const TArray<FUInventoryStruct>& Inventory);

	FPlayerInventoryInfo GetPlayerInventory(AYPPCustomPlayerState* PlayerState);

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

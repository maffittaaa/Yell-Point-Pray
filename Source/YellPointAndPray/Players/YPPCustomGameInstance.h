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
	FUniqueNetIdRepl UniqueId;

	UPROPERTY()
	EPlayerType PlayerType = EPlayerType::None;

	UPROPERTY()
	TSubclassOf<APawn> PawnClass = nullptr;

	UPROPERTY()
	bool bIsHost = false;

	UPROPERTY() // Add this
	FPlayerInventoryInfo InventoryInfo;

	FPlayerInfo() : UniqueId(FUniqueNetIdRepl()), PlayerType(EPlayerType::None), PawnClass(nullptr), bIsHost(false) {}

	FPlayerInfo(const FUniqueNetIdRepl& NetId, EPlayerType NewPlayerType, TSubclassOf<APawn> NewPawnClass, bool IsHost, const FPlayerInventoryInfo& Inventory)
		: UniqueId(NetId), PlayerType(NewPlayerType), PawnClass(NewPawnClass), bIsHost(IsHost), InventoryInfo(Inventory) {
	}
};

UCLASS()
class YELLPOINTANDPRAY_API UYPPCustomGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	void PlayerTravelling(TSubclassOf<APawn> NewPawnClass, EPlayerType NewPlayerType, AYPPCustomPlayerState* PlayerState, const FPlayerInventoryInfo& InventoryInfo);

	void StorePlayerInventory(AYPPCustomPlayerState* PlayerState, TArray<FUInventoryStruct>& Inventory);

	FPlayerInventoryInfo GetPlayerInventory(AYPPCustomPlayerState* PlayerState);

	UPROPERTY()
	AYPPCustomPlayerState* PlayerStateRef = nullptr;
	
	EPlayerType GetPlayerType(AYPPCustomPlayerState* PlayerState);

	TSubclassOf<APawn> GetPlayerClass(AYPPCustomPlayerState* PlayerState);

	void ClearPlayerInfoArray();
	
	TArray<FPlayerInfo> PlayerInfoArray;
protected:

	UPROPERTY()
	EPlayerType DefaultType = EPlayerType::None;

	UPROPERTY(EditDefaultsOnly, Category = "Asymmetric")
	TSubclassOf<APawn> DefaultClass;

	bool GoingUp = true;
	
	int32 GetPlayerInfoIndexByUniqueId(const FUniqueNetIdRepl& UniqueId);

	int32 GetPlayerIndex(AYPPCustomPlayerState* PlayerState);

	virtual void Init() override;
};

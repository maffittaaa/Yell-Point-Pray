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
	FPlayerInventoryInfo(TArray<FUInventoryStruct> Slots) : InventorySlots(Slots) {}
};

USTRUCT(BlueprintType)
struct FPlayerInfo
{
	GENERATED_BODY()

	UPROPERTY()
	int32 UniqueId;

	UPROPERTY()
	EPlayerType PlayerType = EPlayerType::None;

	UPROPERTY()
	TSubclassOf<APawn> PawnClass = nullptr;

	UPROPERTY()
	bool bIsHost = false;

	UPROPERTY() // Add this
	FPlayerInventoryInfo InventoryInfo;

	FPlayerInfo() : UniqueId(0), PlayerType(EPlayerType::None), PawnClass(nullptr), bIsHost(false) {}

	FPlayerInfo(int32 NetId, EPlayerType NewPlayerType, TSubclassOf<APawn> NewPawnClass, bool IsHost, FPlayerInventoryInfo Inventory)
		: UniqueId(NetId), PlayerType(NewPlayerType), PawnClass(NewPawnClass), bIsHost(IsHost), InventoryInfo(Inventory) {
	}
};

UCLASS()
class YELLPOINTANDPRAY_API UYPPCustomGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	void PlayerTravelling(TSubclassOf<APawn> NewPawnClass, EPlayerType NewPlayerType, AYPPCustomPlayerState* PlayerState, FPlayerInventoryInfo InventoryInfo);

	//void StorePlayerInventory(AYPPCustomPlayerState* PlayerState, TArray<FUInventoryStruct>& Inventory);

	FPlayerInventoryInfo GetPlayerInventory(AYPPCustomPlayerState* PlayerState);

	UPROPERTY()
	AYPPCustomPlayerState* PlayerStateRef = nullptr;
	
	EPlayerType GetPlayerType(AYPPCustomPlayerState* PlayerState);

	TSubclassOf<APawn> GetPlayerClass(AYPPCustomPlayerState* PlayerState);

	void ClearPlayerInfoArray();
	
	TArray<FPlayerInfo> PlayerInfoArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PreviewImage")
	TArray<UTexture2D*> PreviewImageList;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PreviewImage")
	UTexture2D* PreviewImageTreasure;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PreviewImage")
	UTexture2D* PreviewImageToyHammer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PreviewImage")
	UTexture2D* PreviewImageWalkieTalkie;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PreviewImage")
	UTexture2D* PreviewImageLockPick;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PreviewImage")
	UTexture2D* PreviewImageRubberDuck;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PreviewImage")
	UTexture2D* PreviewImageFlashlight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PreviewImage")
	UTexture2D* PreviewImageKeys;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PreviewImage")
	UTexture2D* PreviewImageKebab;

protected:

	UPROPERTY()
	EPlayerType DefaultType = EPlayerType::None;

	UPROPERTY(EditDefaultsOnly, Category = "Asymmetric")
	TSubclassOf<APawn> DefaultClass;

	bool GoingUp = true;
	
	int32 GetPlayerInfoIndexByUniqueId(int32 UniqueId);

	int32 GetPlayerIndex(AYPPCustomPlayerState* PlayerState);

	virtual void Init() override;
};

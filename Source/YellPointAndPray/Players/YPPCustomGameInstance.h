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

UCLASS()
class YELLPOINTANDPRAY_API UYPPCustomGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	void PlayerLoggedIn(AYPPCustomPlayerState* NewPlayerState, EPlayerType NewPlayerType);

	UPROPERTY()
	AYPPCustomPlayerState* PlayerStateRef = nullptr;

	UPROPERTY()
	EPlayerType PlayerType = EPlayerType::None;

protected:
	virtual void Init() override;

private:
	void OnMapLoad(UWorld* World);
};

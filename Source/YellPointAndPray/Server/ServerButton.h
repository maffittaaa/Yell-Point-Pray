// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "MatchSessionInfo.h"
#include "ServerButton.generated.h"

/**
 *
 */
UCLASS()
class YELLPOINTANDPRAY_API UServerButton : public UButton
{
	GENERATED_BODY()
public:
	UServerButton();

	void SetSessionInfo(const FMatchSessionInfo& inInfo);

private:
	FMatchSessionInfo SessionInfo;

	UFUNCTION()
	void OnClick();
};

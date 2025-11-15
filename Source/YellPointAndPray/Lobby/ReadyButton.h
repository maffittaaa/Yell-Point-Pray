// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ComicallyLargeButton.h"
#include "ReadyButton.generated.h"

/**
 * 
 */
UCLASS()
class YELLPOINTANDPRAY_API AReadyButton : public AComicallyLargeButton
{
	GENERATED_BODY()
private:
	AReadyButton();
public:
	virtual void Interact_Implementation(AActor* Interactor) override;

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerToggleReadyState();
};

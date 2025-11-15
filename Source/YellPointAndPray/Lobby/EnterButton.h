// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ComicallyLargeButton.h"
#include "EnterButton.generated.h"

/**
 * 
 */
UCLASS()
class YELLPOINTANDPRAY_API AEnterButton : public AComicallyLargeButton
{
	GENERATED_BODY()
public:
	virtual void Interact_Implementation(AActor* Interactor) override;

	UFUNCTION(Server, Reliable)
	void ChangeAvailability(bool State);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerToggleEnterState();
};

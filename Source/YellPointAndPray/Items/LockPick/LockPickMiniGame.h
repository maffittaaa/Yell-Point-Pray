// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/CanvasPanelSlot.h"
#include "Door.h"
#include "LockPickMiniGame.generated.h"
/**
 * 
 */
UCLASS()
class YELLPOINTANDPRAY_API ALockPickMiniGame : public AActor
{
	GENERATED_BODY()
public:
	ALockPickMiniGame();

	void OnClick(AActor* User1);

	AActor* User;
	ADoor* DoorOpening;
	AActor* Owner;

protected:
	
	UWidget* Arrow;
	UWidget* HitSpot;
	UWidget* Bar;

	UCanvasPanelSlot* ArrowSlot;
	UCanvasPanelSlot* HitSpotSlot;
	UCanvasPanelSlot* BarSlot;


	UPROPERTY(EditAnywhere)
	UUserWidget* GameOverWidget;

	float ArrowSpeed = 500.f;
	bool bGoingDown = true;

	float BarTop;
	float BarBottom; 

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	FTimerHandle DelayHandle;

	void EnableControls();

};

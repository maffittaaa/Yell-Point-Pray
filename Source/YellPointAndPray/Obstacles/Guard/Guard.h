// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Perception/PawnSensingComponent.h"
#include "AIController.h"
#include "Guard.generated.h"

UCLASS()
class YELLPOINTANDPRAY_API AGuard : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AGuard();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	UPawnSensingComponent* PawnSensingComp;

	UPROPERTY(EditAnywhere)
	TArray<AActor*> Waypoints;


	UPROPERTY(Replicated, VisibleAnywhere)
	int CurrentWaypoint = 0;

	UPROPERTY(Replicated, VisibleAnywhere)
	bool Patroling = true;
	UPROPERTY(Replicated, VisibleAnywhere)
	bool Suspicious = false;
	UPROPERTY(Replicated, VisibleAnywhere)
	bool Seen = false;

	AAIController* AIController;

	UPROPERTY(Replicated)
	AActor* TargetPlayer;
	FVector LastSeenLocation;

	float ExtraRotation = 0;
	int invert = 1;

	int SuspicionMax = 100;
	UPROPERTY(Replicated)
	int CurrentSuspicion = 0;

	UFUNCTION()
	void Patrol();
	UFUNCTION()
	void SeenPlayer(APawn* Pawn);
	UFUNCTION()
	void SuspiciousOf();

	void LookAround(float DeltaTime);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};

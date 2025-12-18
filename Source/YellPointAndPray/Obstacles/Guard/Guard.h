// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Perception/PawnSensingComponent.h"
#include "Interfaces/Knockable.h"
#include "AIController.h"
#include "Interfaces/Reset.h"
#include "Door.h"
#include "Guard.generated.h"

UENUM(BlueprintType, Category = "Animation")
enum EGuardingStates : uint8
{
	Patrolling,
	Chasing,
	Alerted,
	LookingForPlayer
};

UCLASS()
class YELLPOINTANDPRAY_API AGuard : public ACharacter, public IKnockable, public IReset
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UAudioComponent* CaughtAudioPlayer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundWave* CaughtSound;

	// Sets default values for this character's properties
	AGuard();

	UPROPERTY(ReplicatedUsing = OnRepAnimationState, BlueprintReadWrite, Category = "Animations")
	TEnumAsByte<EGuardingStates> guardingState = Patrolling;

	UFUNCTION()
	void OnRepAnimationState();

	UFUNCTION(BlueprintImplementableEvent)
	void OnAnimationStateModified(EGuardingStates newState);
	
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

	UPROPERTY(EditAnywhere)
	bool Loop = false;
	int Looper = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Suspicious Mark")
	UStaticMeshComponent* suspiciousMark;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Alerted Mark")
	UStaticMeshComponent* alertedMark;

	AAIController* AIController;

	UPROPERTY(Replicated)
	AActor* TargetPlayer;
	FVector LastSeenLocation;

	float ExtraRotation = 0;
	int invert = 1;

	int SuspicionMax = 100;
	UPROPERTY(Replicated)
	float CurrentSuspicion = 0;

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

	FVector StartLocation;
	FQuat StartRotation;

	AActor* LastOpen;
	float CloseTimer = 0;

	UFUNCTION()
	void OpenDoors();
	UFUNCTION()
	void CloseDoors(float DeltaTime);

	UFUNCTION()
	void Called(FVector Location);

	UPROPERTY(Replicated)
	UWorld* World;

	UPROPERTY(Replicated, BlueprintReadWrite, EditAnywhere, Category = "Knocked")
	bool Knocked = false;

	virtual void Reset_Implementation() override;

	virtual void Knock_Implementation() override;

	UFUNCTION(Server, Reliable, WithValidation)
	virtual void ServerKnock() override;

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerUnKnock();

	UFUNCTION()
	void KnockMySelf();

	UFUNCTION()
	void UnKnockMySelf();
};

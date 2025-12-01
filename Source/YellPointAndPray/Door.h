// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/Interactable.h"
#include "Interfaces/Reset.h"
#include <Components/TimelineComponent.h>
#include "Door.generated.h"

UCLASS()
class YELLPOINTANDPRAY_API ADoor : public AActor, public IInteractable, public IReset
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADoor();

	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* DoorMeshComp;
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* FrameMeshComp;
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Mesh;
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* DoorNobMesh;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(Replicated)
	int IsOpen = -1;
	UPROPERTY(Replicated)
	int Side = 1;
	UPROPERTY(Replicated)
	bool Play = false;

	UPROPERTY(EditAnywhere, Replicated)
	bool Locked;

	UPROPERTY(EditAnywhere, Replicated)
	bool isInitiallyLocked;

	UPROPERTY(EditAnywhere)
	int DoorID = -1;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void Interact_Implementation(AActor* Interactor) override;

	virtual void Reset_Implementation() override;

	UFUNCTION(Server, Reliable)
	void ServerInteract(AActor* Interactor);

	UFUNCTION(Server, Reliable)
	void CloseDoor();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastDoor(float FinalYaw);

	UFUNCTION(Server, Reliable)
	void UnlockDoor();

	UFUNCTION(Server, Reliable)
	void KeyUnlockDoor(int KeyID);

	UFUNCTION(Server, Reliable)
	void LockDoor();

	UFUNCTION(Server, Reliable)
	void GetLockDoor();
};

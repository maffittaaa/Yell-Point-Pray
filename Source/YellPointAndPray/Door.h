// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/Interactable.h"
#include <Components/TimelineComponent.h>
#include "Door.generated.h"

UCLASS()
class YELLPOINTANDPRAY_API ADoor : public AActor, public IInteractable
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


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void Interact_Implementation(AActor* Interactor) override;

	UFUNCTION(Server, Reliable)
	void UnlockDoor();
};

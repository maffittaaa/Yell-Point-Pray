// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/Interactable.h"
#include "Obstacles/Laser/Laser.h"
#include "EletricBox.generated.h"

UCLASS()
class YELLPOINTANDPRAY_API AEletricBox : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEletricBox();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* eletricBoxMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Laser")
	TArray<AActor*> laserActors;


	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void Interact_Implementation(AActor* interactor) override;

};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Builders/ConeBuilder.h"
#include "Components/BoxComponent.h"
#include "Containers/Map.h"
#include "GameFramework/Actor.h"
#include "Camera.generated.h"

UCLASS()
class YELLPOINTANDPRAY_API ACamera : public AActor
{
	GENERATED_BODY()

	public:
		// Sets default values for this actor's properties
		ACamera();
		
		UPROPERTY(VisibleAnywhere, Category = "Camera Mesh")
		UStaticMeshComponent* cameraMeshComponent;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision Cone")
		UStaticMeshComponent* collisionCone;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Suspicious Mark")
		UStaticMeshComponent* suspiciousMark;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Alerted Mark")
		UStaticMeshComponent* alertedMark;

	protected:
		// Called when the game starts or when spawned
		virtual void BeginPlay() override;

		int SuspicionMax;
		int32 AmountOfPlayers;
		TMap<AActor*, float>PlayersSeenList;
		TMap<AActor*, float>PlayersNotSeenList;

		UFUNCTION()
		void PlayerInVision(float DeltaTime);

		UFUNCTION()
		void NoPlayerInVision(float DeltaTime);

	public:
		// Called every frame
		virtual void Tick(float DeltaTime) override;

		UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

		UFUNCTION()
		void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};

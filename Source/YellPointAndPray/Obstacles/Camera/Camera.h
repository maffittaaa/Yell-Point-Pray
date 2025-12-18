// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "Containers/Map.h"
#include "GameFramework/Actor.h"
#include "Interfaces/Reset.h"
#include "Components/AudioComponent.h"
#include "Camera.generated.h"

UCLASS(Blueprintable)
class YELLPOINTANDPRAY_API ACamera : public AActor, public IReset
{
	GENERATED_BODY()

	public:
		// Sets default values for this actor's properties
		ACamera();

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Skeletal Mesh")
		USkeletalMeshComponent* skeletalMeshComponent;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision Cone")
		UStaticMeshComponent* collisionCone;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Suspicious Mark")
		UStaticMeshComponent* suspiciousMark;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Alerted Mark")
		UStaticMeshComponent* alertedMark;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Detected Material")
		UMaterial* detectedMaterial;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Undetected Material")
		UMaterial* undetectedMaterial;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Detected Material instance")
		UMaterialInstance* detectedMaterialInstance;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Undetected Material instance")
		UMaterialInstance* undetectedMaterialInstance;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Animation")
		bool bIsAnimationStopped = false;

		float suspiciousAmount = 0.0f;

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
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
		UAudioComponent* CameraAudioPlayer;

		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USoundWave* CameraSound;

		// Called every frame
		virtual void Tick(float DeltaTime) override;

		virtual void Reset_Implementation() override;

		UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

		UFUNCTION()
		void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

		void StopAnimation();
		void ResetCameraObstacle();
		void ResumeCameraAnimation();
};

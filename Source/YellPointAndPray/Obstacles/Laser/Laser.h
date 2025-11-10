// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "YellPointAndPrayCharacter.h"
#include "NiagaraComponent.h"
#include "Interfaces/Reset.h"
#include "Laser.generated.h"

UCLASS()
class YELLPOINTANDPRAY_API ALaser : public AActor, public IReset
{
	GENERATED_BODY()
	
	public:
		ALaser();

		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		UStaticMeshComponent* laserMesh;

		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		UNiagaraComponent* niagaraLaser;

		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		UNiagaraComponent* niagaraLaserImpact;

		UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
		UNiagaraSystem* niagaraLaserSystem;

		UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
		UNiagaraSystem* niagaraLaserImpactSystem;
		
		UFUNCTION(BlueprintCallable, Category = "Laser")
		void SetLaserColors();

		void TouchingLaser(AYellPointAndPrayCharacter* character);

		UFUNCTION(BlueprintCallable, Category = "Laser")
		void SetLaserActive(bool bActive);

		UFUNCTION(BlueprintCallable, Category = "Laser")
		bool IsLaserActive() const { return bIsLaserActive; }

		UFUNCTION()
		void DeactivateLaserTemporarily(float duration = 3.0f);

		FTimerHandle reactivateTimerHandle;

		UFUNCTION()
		void ReactivateLasers();

	private:
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Laser", meta = (AllowPrivateAccess = "true"))
		bool bIsLaserActive = true;

	protected: 
		virtual void BeginPlay() override;

		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components");
		USphereComponent* collisionSphere;

	public:	
		// Called every frame
		virtual void Reset_Implementation() override;

		virtual void Tick(float DeltaTime) override;

};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/Interactable.h"
#include "WhiteBoard.generated.h"

UCLASS()
class YELLPOINTANDPRAY_API AWhiteBoard : public AActor, public IInteractable
{
	GENERATED_BODY()
	
	public:	
		AWhiteBoard();
	
		UPROPERTY(VisibleAnywhere)
		UStaticMeshComponent* meshComp;

		
		UPROPERTY()
		UMaterialInstanceDynamic* dynamicMaterialInstance;

		UPROPERTY(EditAnywhere, Category = "Materials")
		UTextureRenderTarget2D* renderTargetWhiteboard;

		UPROPERTY(EditAnywhere, Category = "Materials")
		UMaterialInterface* markerP1Material;

		UPROPERTY(EditAnywhere, Category = "Materials")
		UMaterialInterface* markerP2Material;

		UPROPERTY(EditAnywhere, Category = "Materials")
		UMaterialInterface* markerP3Material;

		void DrawOnWhiteboard(FVector2D* locationToDraw);
	
	protected:
		virtual void BeginPlay() override;

	public:	
		// Called every frame
		virtual void Tick(float DeltaTime) override;

		virtual void Interact_Implementation(AActor* interactor) override;

		UFUNCTION(BlueprintCallable)
		void CloseBoard();
};

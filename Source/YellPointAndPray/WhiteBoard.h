// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/Interactable.h"
#include "Camera/CameraActor.h"
#include "WhiteBoard.generated.h"

UCLASS()
class YELLPOINTANDPRAY_API AWhiteBoard : public AActor, public IInteractable
{
	GENERATED_BODY()
	
	public:	
		AWhiteBoard();
	
		UPROPERTY(VisibleAnywhere)
		UStaticMeshComponent* meshComp;

		UPROPERTY(EditAnywhere, Category = "WhiteboardRenderTarget")
		UTextureRenderTarget2D* renderTarget2D;

		UPROPERTY(EditAnywhere, Category = "WhiteboardMaterial")
		UMaterialInstanceDynamic* dynamicMaterialInstanceCanvas;

		UPROPERTY(EditAnywhere, Category = "WhiteboardMaterial")
		UMaterialInstanceDynamic* dynamicMaterialInstanceBrush;

		UPROPERTY(EditAnywhere, Category = "WhiteboardMaterial")
		UMaterial* materialCanvas;

		UPROPERTY(EditAnywhere, Category = "WhiteboardMaterial")
		UMaterial* materialBrush;
	
	protected:
		virtual void BeginPlay() override;

	public:	
		// Called every frame
		virtual void Tick(float DeltaTime) override;

		virtual void Interact_Implementation(AActor* interactor) override;

		void Draw(UTexture2D* brushTexture, float brushSize, FVector2D* drawLocation);

		UFUNCTION(BlueprintCallable)
		void CloseBoard();
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "Interfaces/Interactable.h"
#include "Components/Button.h"
#include "Players/YPPCustomPlayerState.h"
#include "WhiteBoard.generated.h"

UCLASS()
class YELLPOINTANDPRAY_API AWhiteBoard : public AActor, public IInteractable
{
	GENERATED_BODY()
	
	public:	
		AWhiteBoard();
	
		UPROPERTY(VisibleAnywhere)
		UStaticMeshComponent* meshComp;

		UPROPERTY(EditAnywhere, Category = "Whiteboard")
		UStaticMeshComponent* cubeMeshComp;

		UPROPERTY(EditAnywhere, Category = "WhiteboardRenderTarget")
		UTextureRenderTarget2D* renderTarget2D;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WhiteboardMaterial")
		UMaterialInstanceDynamic* dynamicMaterialInstanceCanvas;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WhiteboardMaterial")
		UTexture2D* canvasTexture;

		UPROPERTY(EditAnywhere, Category = "WhiteboardMaterial")
		UMaterialInstanceDynamic* dynamicMaterialInstanceBrush;

		UPROPERTY(EditAnywhere, Category = "WhiteboardMaterial")
		UMaterial* materialCanvas;
		
		UPROPERTY(EditAnywhere, Category = "WhiteboardMaterial")
		UMaterial* materialBrush;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WhiteboardBrush")
		UTexture2D* brushTexture_P1;
	
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WhiteboardBrush")
		UTexture2D* brushTexture_P2;
	
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WhiteboardBrush")
		UTexture2D* brushTexture_P3;

		// //closing board
		// UPROPERTY()
		// UUserWidget* closingWidget;
		//
		// UPROPERTY(EditAnywhere, Category = "Whiteboard")
		// TSubclassOf<UUserWidget> closingWidgetClass;
	
		// UFUNCTION()
		// void CloseWidget();
	
	protected:
		virtual void BeginPlay() override;

	public:	
		// Called every frame
		virtual void Tick(float DeltaTime) override;

		virtual void Interact_Implementation(AActor* interactor) override;

		void Draw(UTexture2D* brushTexture, float brushSize, FVector2D drawLocation, AYPPCustomPlayerState* playerState);

		void InitializeBackground();

		UFUNCTION(BlueprintCallable, Server, Reliable)
		void CloseBoard();
};

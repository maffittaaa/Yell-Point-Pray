// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/Interactable.h"
#include "ComicallyLargeButton.generated.h"

UCLASS()
class YELLPOINTANDPRAY_API AComicallyLargeButton : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AComicallyLargeButton();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* BigButtonMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* BaseButtonMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* SuportButtonMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
	UMaterialInstance* PreparedToBeUsed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
	UMaterialInstance* AlreadyUsed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
	UMaterialInstance* CantBeUsed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Owner Player")
	TSubclassOf<AActor> PlayerReady;

	UPROPERTY(ReplicatedUsing = OnRep_ButtonState, BlueprintReadOnly, Category = "Button State")
	bool ButtonUsed = false;

	UPROPERTY(ReplicatedUsing = OnRep_ButtonState, BlueprintReadOnly, Category = "Button State")
	bool ButtonAvailable = false;

	// Add this function to handle replication
	UFUNCTION()
	void OnRep_ButtonState();

	// Override GetLifetimeReplicatedProps
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};

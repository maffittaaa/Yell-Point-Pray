// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/Usable.h"
#include "GameFramework/Actor.h"
#include "UsableItem.generated.h"

UCLASS()
class YELLPOINTANDPRAY_API AUsableItem : public AActor, public IUsable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AUsableItem();

	UPROPERTY()
	int ID = 0;

	UPROPERTY()
	int KeyID = -1;

	FString Name = "DefaultItemName";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PreviewImage")
	UTexture2D* PreviewImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ObjectToSpawn")
	UClass* Obj;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Mesh;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};

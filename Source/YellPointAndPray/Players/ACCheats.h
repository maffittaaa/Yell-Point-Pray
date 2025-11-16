// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ACCheats.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class YELLPOINTANDPRAY_API UACCheats : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UACCheats();
	
	UPROPERTY(ReplicatedUsing=OnRep_NotDetectedByGuards, BlueprintReadWrite, Category = "Cheats")
	bool bNotDetectedByGuards = false;
	
	UFUNCTION()
	void NotDetectedByGuard();

	UFUNCTION()
	void TeleportToLaserRoom(AActor* character, FVector location);

	UFUNCTION()
	void TeleportToEletricalRoom(AActor* character, FVector location);

	// UFUNCTION()
	// void SpawnMoreItems();

	UFUNCTION(Server, Reliable)
	void Server_SetNotDetectedByGuard(bool bNewState);

	UFUNCTION()
	void OnRep_NotDetectedByGuards();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};

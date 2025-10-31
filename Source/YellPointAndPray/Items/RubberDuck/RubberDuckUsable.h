// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/UsableItemsParent/UsableItem.h"
#include "RubberDuckUsable.generated.h"

/**
 * 
 */
UCLASS()
class YELLPOINTANDPRAY_API ARubberDuckUsable : public AUsableItem
{
	GENERATED_BODY()
	
private:
	ARubberDuckUsable();

	UPROPERTY(Replicated)
	bool AddForce = false;

	int ThrowForce = 0;

protected:
	virtual void BeginPlay() override;

public:
	virtual void Use_Implementation(AActor* User) override;


	UFUNCTION(Server, Reliable)
	void Throw(AActor* User, UWorld* World, TSubclassOf<AActor> NewHoldingItemClass, FVector NewPosition, FRotator dir);

	void ChangeAdd();

	virtual void Tick(float DeltaTime) override;
};

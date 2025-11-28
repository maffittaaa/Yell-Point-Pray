// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/UsableItemsParent/UsableItem.h"
#include "KeyUsable.generated.h"

/**
 * 
 */
UCLASS()
class YELLPOINTANDPRAY_API AKeyUsable : public AUsableItem
{
	GENERATED_BODY()
	
private:
	AKeyUsable();
public:
	UPROPERTY(Replicated)
	int KeyID = -1;

	virtual void Use_Implementation(AActor* User) override;


	UFUNCTION(Server, Reliable)
	void UseReal(AActor* User, UWorld* World);

	void BeginPlay();
};

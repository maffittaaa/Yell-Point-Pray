// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/UsableItemsParent/UsableItem.h"
#include "UsableLockPick.generated.h"

/**
 * 
 */
UCLASS()
class YELLPOINTANDPRAY_API AUsableLockPick : public AUsableItem
{
	GENERATED_BODY()
	
private:
	AUsableLockPick();
public:
		virtual void Use_Implementation(AActor* User) override;


		UFUNCTION(Server, Reliable)
		void UseReal(AActor* User, UWorld* World);
};

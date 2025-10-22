// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/UsableItemsParent/UsableItem.h"
#include "ToyHammerUsable.generated.h"

/**
 * 
 */
UCLASS()
class YELLPOINTANDPRAY_API AToyHammerUsable : public AUsableItem
{
	GENERATED_BODY()
private:
	AToyHammerUsable();
public:
	virtual void Use_Implementation(AActor* User) override;
};

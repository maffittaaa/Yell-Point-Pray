// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/PickUpItems/Test/PickableItem.h"
#include "Interfaces/Usable.h"
#include "ToyHammer.generated.h"

/**
 * 
 */
UCLASS()
class YELLPOINTANDPRAY_API AToyHammer : public APickableItem, public IUsable
{
	GENERATED_BODY()

private:
	AToyHammer();

public:

	virtual void Use_Implementation(AActor* User) override;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/UsableItemsParent/UsableItem.h"
#include "WalkieTalkieUsable.generated.h"

/**
 * 
 */
UCLASS()
class YELLPOINTANDPRAY_API AWalkieTalkieUsable : public AUsableItem
{
	GENERATED_BODY()
private:
	AWalkieTalkieUsable();
public:
	virtual void Use_Implementation(AActor* User) override;
};

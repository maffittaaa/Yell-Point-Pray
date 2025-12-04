// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/UsableItemsParent/UsableItem.h"
#include "KebabUsable.generated.h"

/**
 * 
 */
UCLASS()
class YELLPOINTANDPRAY_API AKebabUsable : public AUsableItem
{
	GENERATED_BODY()
private:
	AKebabUsable();

	virtual void Use_Implementation(AActor* User) override;
};

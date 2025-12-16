// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/PickableItemsParent/PickableItem.h"
#include "KeyPickable.generated.h"

/**
 * 
 */
UCLASS()
class YELLPOINTANDPRAY_API AKeyPickable : public APickableItem
{
	GENERATED_BODY()

private:
	AKeyPickable();
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	int KeyID = -1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	FString KeyName = "";
};

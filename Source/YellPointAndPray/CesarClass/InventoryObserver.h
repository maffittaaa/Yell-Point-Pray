// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InventoryObserver.generated.h"

/**
 *
 */
UINTERFACE(MinimalAPI, Blueprintable)
class UInventoryObserver : public UInterface
{
	GENERATED_BODY()
};

class IInventoryObserver {
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent)
	void OnItemAdded(const FString& ItemName);

protected:

};

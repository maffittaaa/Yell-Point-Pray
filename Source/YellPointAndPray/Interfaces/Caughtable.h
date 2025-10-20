// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Caughtable.generated.h"

UINTERFACE(BlueprintType)
class YELLPOINTANDPRAY_API UCaughtable : public UInterface
{
	GENERATED_BODY()
};

class ICaughtable {
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Damage")
	void Caught();
};
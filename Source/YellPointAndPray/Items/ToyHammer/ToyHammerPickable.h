// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/PickableItemsParent/PickableItem.h"
#include "Components/AudioComponent.h"
#include "ToyHammerPickable.generated.h"

/**
 * 
 */
UCLASS()
class YELLPOINTANDPRAY_API AToyHammerPickable : public APickableItem
{
	GENERATED_BODY()

private:
	AToyHammerPickable();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UAudioComponent* ToyHammerAudioPlayer;
};

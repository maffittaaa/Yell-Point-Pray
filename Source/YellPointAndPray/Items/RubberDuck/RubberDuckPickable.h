// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/PickableItemsParent/PickableItem.h"
#include "RubberDuckPickable.generated.h"

/**
 * 
 */
UCLASS()
class YELLPOINTANDPRAY_API ARubberDuckPickable : public APickableItem
{
	GENERATED_BODY()

private:
	ARubberDuckPickable();

	void CallGuard();
	
	bool HasQuacked = false;


protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USphereComponent* SphereCollider;
public:

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};

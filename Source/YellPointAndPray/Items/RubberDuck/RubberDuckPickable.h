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
	
	UPROPERTY(Replicated)
	bool HasQuacked = false;


protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USphereComponent* SphereCollider;
public:

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor,UPrimitiveComponent* OtherComp, FVector NormalImpulse,const FHitResult& Hit);
};

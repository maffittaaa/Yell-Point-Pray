// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/PickableItemsParent/PickableItem.h"
#include "Components/AudioComponent.h"
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
	UPROPERTY(VisibleAnywhere, Replicated, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UAudioComponent* DuckAudioPlayer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	TArray<USoundWave*> QuackSoundsList;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	bool CanMakeSound = false;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor,UPrimitiveComponent* OtherComp, FVector NormalImpulse,const FHitResult& Hit);
};

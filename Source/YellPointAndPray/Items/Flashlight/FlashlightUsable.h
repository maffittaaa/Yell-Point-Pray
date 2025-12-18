// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/UsableItemsParent/UsableItem.h"
#include <Components/SpotLightComponent.h>
#include "YellPointAndPrayCharacter.h"
#include "FlashlightUsable.generated.h"

/**
 * 
 */
UCLASS()
class YELLPOINTANDPRAY_API AFlashlightUsable : public AUsableItem
{
	GENERATED_BODY()
private:
	AFlashlightUsable();

	UPROPERTY(VisibleAnywhere)
	USpotLightComponent* Light;

	UPROPERTY(ReplicatedUsing = OnRep_IsLightOn)
	bool bIsLightOn;

	UFUNCTION()
	void OnRep_IsLightOn();

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UAudioComponent* FlashlightAudioPlayer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundWave* FlashlightSound;

	virtual void Use_Implementation(AActor* User) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(Server, Reliable)
	void Server_ToggleLight();

	UPROPERTY(Replicated)
	AYellPointAndPrayCharacter* UserStored;
};

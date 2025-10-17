#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Inventory.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class YELLPOINTANDPRAY_API UInventory : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInventory();
	int InventorySlots[3];

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	bool IsInventoryFull();

	int GetInventorySize();

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	int GetSlotItem(int SlotID);

	UFUNCTION()
	void SetInventory(int ItemID);
		
};

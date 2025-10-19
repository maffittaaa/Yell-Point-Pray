#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include <string>
#include "Inventory.generated.h"

using namespace std;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class YELLPOINTANDPRAY_API UInventory : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInventory();
	UTexture2D* InventorySlots[3];

	bool IsInventoryFull();
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	

	int GetInventorySize();

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
	void SetInventory(int ItemID, UTexture2D* PreviewImage);
	
	UFUNCTION(BlueprintCallable)
	UTexture2D* GetSlotItem(int SlotID);
};

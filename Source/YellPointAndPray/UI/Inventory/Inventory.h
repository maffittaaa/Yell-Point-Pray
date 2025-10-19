#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include <string>
#include "Inventory.generated.h"

using namespace std;

//struct UInventoryStruct
//{
//	int ID = -1;
//	UTexture2D* PreviewImage = nullptr;
//};

USTRUCT(BlueprintType)
struct FUInventoryStruct
{
	GENERATED_BODY()

public:

	UPROPERTY()
	int32 ID = -1;

	UPROPERTY()
	UTexture2D* PreviewImage = nullptr;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class YELLPOINTANDPRAY_API UInventory : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInventory();

protected:
	static const int InventorySize = 3;

	UPROPERTY(Replicated)
	TArray<FUInventoryStruct> InventorySlots;

	// Called when the game starts
	virtual void BeginPlay() override;


public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	

	bool IsInventoryFull();
	
	int GetInventorySize();

	UFUNCTION()
	void SetInventory(int ItemID, UTexture2D* PreviewImage);
	
	UFUNCTION(BlueprintCallable)
	UTexture2D* GetSlotItem(int SlotID);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};

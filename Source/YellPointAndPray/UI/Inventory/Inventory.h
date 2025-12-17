#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Items/PickableItemsParent/PickableItem.h"
#include "Items/UsableItemsParent/UsableItem.h"
#include "Inventory.generated.h"

USTRUCT(BlueprintType)
struct FUInventoryStruct
{
	GENERATED_BODY()

public:

	UPROPERTY()
	int32 ID = -1;

	UPROPERTY()
	int KeyID = -1;

	UPROPERTY()
	FString KeyName = "";

	UPROPERTY()
	FString Name = "DefaultName";

	UPROPERTY()
	UTexture2D* PreviewImage = nullptr;

	UPROPERTY()
	TSubclassOf<AUsableItem> Item = nullptr;
};

USTRUCT(BlueprintType)
struct FInventoryTravelData
{
	GENERATED_BODY()

public:

	UPROPERTY()
	TSubclassOf<AUsableItem> ItemClass;
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

	void ResetSlotToDefaultValue(int SlotID);

	TArray<FUInventoryStruct> TravelInventory;
	
public:	
	TArray<FUInventoryStruct> GetAllInventory();
	
	void StoreInitialInventory(TArray<FUInventoryStruct> InitialInventory);
	
	void RestoreInventoryWithTravelData(const TArray<FUInventoryStruct>& TravelData);

	//void RestoreInventoryWithInitalItems();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated)
	int CurrentItemSelected = 0;

	bool IsInventoryFull();
	
	int GetInventorySize();

	int GetSlotID(int SlotID);

	int GetSlotKeyID(int SlotID);

	UFUNCTION(BlueprintCallable)
	FString GetSlotKeyName(int SlotID);

	FString GetSlotName(int SlotID);
	
	TSubclassOf<AUsableItem> GetSlotObj(int SlotID);

	UFUNCTION()
	void SetInventory(APickableItem* Item);
	
	UFUNCTION(Server, Reliable)
	void DeleteInventorySlot(int SlotID);

	UFUNCTION(BlueprintCallable)
	UTexture2D* GetSlotItem(int SlotID);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};

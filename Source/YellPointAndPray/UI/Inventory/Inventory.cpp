#include "Inventory.h"
#include <string>
#include <Net/UnrealNetwork.h>


using namespace std;

UInventory::UInventory()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);

	InventorySlots.SetNum(InventorySize);
	// ...
}

void UInventory::BeginPlay()
{
	Super::BeginPlay();

}

int UInventory::GetInventorySize() 
{
	return InventorySize;
}

bool UInventory::IsInventoryFull()
{
	int quantity = 0;
	for (FUInventoryStruct slot : InventorySlots)
	{
		if (slot.ID != -1)
		{
			//UE_LOG(LogTemp, Warning, TEXT("Slot Content Name: %s"), *slot->GetName());
			quantity++;
		}
	}

	if (quantity >= GetInventorySize()) 
	{
		return true;
	}

	return false;
}

void UInventory::SetInventory(APickableItem* Item)
{
	for (int i = 0; i < GetInventorySize(); i++)
	{
		if (InventorySlots[i].ID == -1)
		{
			//UE_LOG(LogTemp, Warning, TEXT("Item added to slot"));
			InventorySlots[i].ID = Item->ID;
			InventorySlots[i].Item = Item->GetClass();
			InventorySlots[i].PreviewImage = Item->PreviewImage;
			break;
		}
	}
}

UClass* UInventory::GetSlotObj(int SlotID)
{
	return InventorySlots[SlotID].Item;
}

int UInventory::GetSlotID(int SlotID)
{
	return InventorySlots[SlotID].ID;
}

UTexture2D* UInventory::GetSlotItem(int SlotID)
{
	if (SlotID < 0 || SlotID >= GetInventorySize())
	{
		UE_LOG(LogTemp, Warning, TEXT("GetSlotItem: SlotID %d out of range"), SlotID);
		return nullptr;
	}

	if (InventorySlots[SlotID].ID == -1) // empty
	{
		return nullptr;
	}

	return InventorySlots[SlotID].PreviewImage;
}

// Called every frame
void UInventory::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UInventory::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UInventory, InventorySlots);
}


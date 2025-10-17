#include "Inventory.h"

UInventory::UInventory()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void UInventory::BeginPlay()
{
	Super::BeginPlay();

}

int UInventory::GetInventorySize() 
{
	return sizeof(InventorySlots) / sizeof(int);
}

bool UInventory::IsInventoryFull()
{
	int quantity = 0;
	for (int slot : InventorySlots) 
	{
		if (slot != 0) 
		{
			UE_LOG(LogTemp, Warning, TEXT("Slot id: %i"), slot);
			quantity++;
		}
	}
	if (quantity >= GetInventorySize()) 
	{
		return true;
	}

	return false;
}

void UInventory::SetInventory(int ItemID)
{
	if (!IsInventoryFull())
	{
		for (int i = 0; i < GetInventorySize(); i++)
		{
			UE_LOG(LogTemp, Warning, TEXT("Slot Content: %d"), InventorySlots[i]);

			if (InventorySlots[i] == 0)
			{
				UE_LOG(LogTemp, Warning, TEXT("Item added to slot"));
				InventorySlots[i] = ItemID;
				break;
			}
		}
	}
	else
	{
		//SHOW THAT INVENTORY IS FULL
	}
}

int UInventory::GetSlotItem(int SlotID)
{
	if (SlotID < 0 || SlotID >= GetInventorySize())
	{
		UE_LOG(LogTemp, Warning, TEXT("GetSlotItem: SlotID %d out of range"), SlotID);
		return 0;
	}

	return InventorySlots[SlotID];
}


// Called every frame
void UInventory::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}


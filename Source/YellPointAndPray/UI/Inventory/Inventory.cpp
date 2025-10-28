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

void UInventory::DeleteInventorySlot_Implementation(int SlotID)
{
	if (InventorySlots[SlotID].Item == nullptr) 
	{
		UE_LOG(LogTemp, Warning, TEXT("No item to Delete")); 
		return;
	}

	FDetachmentTransformRules DetachRules(EDetachmentRule::KeepWorld, true);
	InventorySlots[SlotID].Item->DetachFromActor(DetachRules);
	InventorySlots[SlotID].Item->Destroy();
	ResetSlotToDefaultValue(SlotID);
}

void UInventory::ResetSlotToDefaultValue(int SlotID)
{
	InventorySlots[SlotID].Item = nullptr;
	InventorySlots[SlotID].ID = -1;
	InventorySlots[SlotID].Name = "DefaultName";
	InventorySlots[SlotID].PreviewImage = nullptr;
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
			if (Item->Obj) 
			{
				AUsableItem* UsableItem = Cast<AUsableItem>(Item->Obj->GetDefaultObject());

				if (Item->Obj->IsChildOf(AUsableItem::StaticClass()))
				{
					InventorySlots[i].Item = UsableItem;
					InventorySlots[i].ID = UsableItem->ID;
					InventorySlots[i].Name = UsableItem->Name;
					InventorySlots[i].PreviewImage = UsableItem->PreviewImage;
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("Item is not a Usable item class"));
					UE_LOG(LogTemp, Warning, TEXT("Actual parent class: %s"), *Item->Obj->GetSuperClass()->GetName());
				}
			}
			else 
			{
				InventorySlots[i].ID = Item->ID;
				InventorySlots[i].Name = Item->Name;
				InventorySlots[i].PreviewImage = Item->PreviewImage;
			}
			
			break;
		}
	}
}

AUsableItem* UInventory::GetSlotObj(int SlotID)
{
	return InventorySlots[SlotID].Item;
}

int UInventory::GetSlotID(int SlotID)
{
	return InventorySlots[SlotID].ID;
}

FString UInventory::GetSlotName(int SlotID)
{
	return InventorySlots[SlotID].Name;
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


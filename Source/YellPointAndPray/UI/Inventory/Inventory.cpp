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

void UInventory::StoreInitialInventory(TArray<FUInventoryStruct> InitialInventory)
{
	// Store a copy of the current inventory for travel
	TravelInventory = InitialInventory;

	UE_LOG(LogTemp, Warning, TEXT("Stored travel inventory with %d slots"), TravelInventory.Num());
}

void UInventory::RestoreInventoryWithTravelData(const TArray<FUInventoryStruct>& TravelData)
{
	// Clear current inventory
	for (int i = 0; i < InventorySlots.Num(); i++)
	{
		if (InventorySlots[i].Item != nullptr)
		{
			FDetachmentTransformRules DetachRules(EDetachmentRule::KeepWorld, true);
			InventorySlots[i].Item->DetachFromActor(DetachRules);
			InventorySlots[i].Item->Destroy();
		}
		ResetSlotToDefaultValue(i);
	}

	// Restore from travel data
	for (int i = 0; i < TravelData.Num() && i < InventorySlots.Num(); i++)
	{
		InventorySlots[i] = TravelData[i];
	}

	UE_LOG(LogTemp, Warning, TEXT("Restored inventory from travel data with %d slots"), TravelData.Num());
}

TArray<FUInventoryStruct> UInventory::GetAllInventory()
{
	return InventorySlots;
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
					InventorySlots[i].KeyID = UsableItem->KeyID;
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

int UInventory::GetSlotKeyID(int SlotID)
{
	return InventorySlots[SlotID].KeyID;
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
	DOREPLIFETIME(UInventory, CurrentItemSelected);
}


#include "Inventory.h"
#include <string>
#include <Net/UnrealNetwork.h>
#include "Items/Keys/KeyUsable.h"
#include "Items/Keys/KeyPickable.h"
#include <Players/YPPCustomGameInstance.h>


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
	UE_LOG(LogTemp, Warning, TEXT("Start Restoring inventory"));
	for (int i = 0; i < InventorySlots.Num(); i++)
	{
		UE_LOG(LogTemp, Warning, TEXT("First Item: %d"), i);
		if (InventorySlots[i].Item != nullptr)
		{
			FDetachmentTransformRules DetachRules(EDetachmentRule::KeepWorld, true);

			AUsableItem* UsableItem = Cast<AUsableItem>(InventorySlots[i].Item->GetDefaultObject());

			UsableItem->DetachFromActor(DetachRules);
			UsableItem->Destroy();
		}
		ResetSlotToDefaultValue(i);
	}

	UE_LOG(LogTemp, Warning, TEXT("First Round Over"));

	// Restore from travel data
	for (int i = 0; i < TravelData.Num() && i < InventorySlots.Num(); i++)
	{
		UE_LOG(LogTemp, Warning, TEXT("Second Item: %d"), i);
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

	AUsableItem* UsableItem = Cast<AUsableItem>(InventorySlots[SlotID].Item->GetDefaultObject());

	UsableItem->DetachFromActor(DetachRules);
	UsableItem->Destroy();
	ResetSlotToDefaultValue(SlotID);
}

void UInventory::ResetSlotToDefaultValue(int SlotID)
{
	InventorySlots[SlotID].Item = nullptr;
	InventorySlots[SlotID].ID = -1;
	InventorySlots[SlotID].Name = "DefaultName";
	InventorySlots[SlotID].KeyName = "";
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
			if (UGameInstance* GameInstance = GetWorld()->GetGameInstance())
			{
				if (UYPPCustomGameInstance* CustomGameInstance = Cast<UYPPCustomGameInstance>(GameInstance))
				{
					UE_LOG(LogTemp, Warning, TEXT("[Inventory] UsableItems Array: %d"), CustomGameInstance->ItemsUsableArray.Num());

					FActorSpawnParameters SpawnParams;
					AActor* UsableItemActor = GetWorld()->SpawnActor<AActor>(CustomGameInstance->ItemsUsableArray[Item->ID], FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);;

					AUsableItem* UsableItem = Cast<AUsableItem>(UsableItemActor);

					if (UsableItem) 
					{
						if (CustomGameInstance->ItemsUsableArray[Item->ID]->IsChildOf(AUsableItem::StaticClass()))
						{
							InventorySlots[i].Item = CustomGameInstance->ItemsUsableArray[Item->ID];
							InventorySlots[i].ID = UsableItem->ID;
							InventorySlots[i].Name = UsableItem->Name;

							if (AKeyPickable* UsableKey = Cast<AKeyPickable>(Item)) {
								InventorySlots[i].KeyID = UsableKey->KeyID;
								InventorySlots[i].KeyName = UsableKey->KeyName;
							}
						}
						else
						{
							UE_LOG(LogTemp, Warning, TEXT("Item is not a Usable item class"));
							UE_LOG(LogTemp, Warning, TEXT("Actual parent class: %s"), *CustomGameInstance->ItemsUsableArray[Item->ID]->GetSuperClass()->GetName());
						}
					}

					UsableItemActor->Destroy();
				}
			}
			break;
		}
	}
}

TSubclassOf<AUsableItem> UInventory::GetSlotObj(int SlotID)
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

FString UInventory::GetSlotKeyName(int SlotID)
{
	return InventorySlots[SlotID].KeyName;
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

	if (UGameInstance* GameInstance = GetWorld()->GetGameInstance()) 
	{
		if (UYPPCustomGameInstance* CustomGameInstance = Cast<UYPPCustomGameInstance>(GameInstance)) 
		{
			UE_LOG(LogTemp, Warning, TEXT("GetSlotItem: SlotID %d"), InventorySlots[SlotID].ID);
			UE_LOG(LogTemp, Warning, TEXT("GetSlotItem: PreviewImageList Num(): %d"), CustomGameInstance->PreviewImageList.Num());

			return CustomGameInstance->PreviewImageList[InventorySlots[SlotID].ID];
		}
	}

	return nullptr;
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


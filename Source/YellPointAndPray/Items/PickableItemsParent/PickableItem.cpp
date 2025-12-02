// Fill out your copyright notice in the Description page of Project Settings.

#include "PickableItem.h"
#include "GameFramework/Character.h"
#include <Kismet/GameplayStatics.h>
#include <CesarClass/InventoryEventQueueSubsystem.h>

// Sets default values
APickableItem::APickableItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Mesh->SetupAttachment(RootComponent);
	// Mesh->SetSimulatePhysics(false);
	// Mesh->SetMassOverrideInKg(NAME_None, 30.f, true);

	bReplicates = true;
	bAlwaysRelevant = true;
	SetReplicateMovement(true);
}

// Called when the game starts or when spawned
void APickableItem::BeginPlay()
{
	Super::BeginPlay();

	Mesh->SetIsReplicated(true);
	Mesh->bReplicatePhysicsToAutonomousProxy = true;
	
    if (HasAuthority()) {
        Mesh->SetSimulatePhysics(true);
        Mesh->SetMassOverrideInKg(NAME_None, 30.f, true);
    }
}

// Called every frame
void APickableItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APickableItem::Interact_Implementation(AActor* Interactor) 
{
	UE_LOG(LogTemp, Warning, TEXT("Item interacted with! :D"));
	IInventorySubject::Execute_NotifyObservers(this, "FDS");
	Destroy();
}


//Cesar Stuff ----------------------------------------------------------------------

void APickableItem::AddObserver_Implementation(const TScriptInterface<IInventoryObserver>& Observer)
{
    if (Observer && !Observers.Contains(Observer))
    {
        Observers.Add(Observer);
    }
}

void APickableItem::RemoveObserver_Implementation(const TScriptInterface<IInventoryObserver>& Observer)
{
    Observers.Remove(Observer);
}

void APickableItem::NotifyObservers_Implementation(const FString& ItemName)
{
    if (UWorld* World = GetWorld())
    {
        if (UGameInstance* GI = World->GetGameInstance())
        {
            UInventoryEventQueueSubsystem* Queue = GI->GetSubsystem<UInventoryEventQueueSubsystem>();

            for (auto& Observer : Observers)
            {
                if (Observer.GetObject())
                {
                    FInventoryEvent Event;
                    Event.Observer = Observer;
                    Event.ItemName = ItemName;

                    Queue->EnqueueEvent(Event);
                }
            }
        }
    }
}
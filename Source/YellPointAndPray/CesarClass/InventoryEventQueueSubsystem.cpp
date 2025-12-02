// Fill out your copyright notice in the Description page of Project Settings.


#include "CesarClass/InventoryEventQueueSubsystem.h"

void UInventoryEventQueueSubsystem::EnqueueEvent(const FInventoryEvent& Event)
{
    EventQueue.Enqueue(Event);
}

void UInventoryEventQueueSubsystem::Tick(float DeltaTime)
{
    FInventoryEvent Ev;

    while (EventQueue.Dequeue(Ev))
    {
        if (Ev.Observer.GetObject())
        {
            IInventoryObserver::Execute_OnItemAdded(Ev.Observer.GetObject(), Ev.ItemName);
            UE_LOG(LogTemp, Warning, TEXT("QUEUEEUEUEUEUEUEUEUEEUEUEUEUEEUEUEUEEUEUEUEREUEU"));
        }
    }
}

TStatId UInventoryEventQueueSubsystem::GetStatId() const
{
    RETURN_QUICK_DECLARE_CYCLE_STAT(UInventoryEventQueueSubsystem, STATGROUP_Tickables);
}

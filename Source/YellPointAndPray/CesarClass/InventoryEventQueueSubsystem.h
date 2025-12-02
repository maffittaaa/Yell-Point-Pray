#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "CesarClass/InventorySubject.h"
#include "CesarClass/InventoryObserver.h"
#include "InventoryEventQueueSubsystem.generated.h"

USTRUCT()
struct FInventoryEvent
{
    GENERATED_BODY()

    UPROPERTY()
    TScriptInterface<IInventoryObserver> Observer;

    UPROPERTY()
    FString ItemName;
};

UCLASS()
class YELLPOINTANDPRAY_API UInventoryEventQueueSubsystem : public UGameInstanceSubsystem, public FTickableGameObject
{
    GENERATED_BODY()

public:
    void EnqueueEvent(const FInventoryEvent& Event);

    virtual void Tick(float DeltaTime);
    virtual TStatId GetStatId() const override;

    bool IsTickable() const override { return true; }

private:
    TQueue<FInventoryEvent> EventQueue;
};

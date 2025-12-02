#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyMovingDirtyActor.generated.h"

UCLASS()
class AMyMovingDirtyActor : public AActor
{
    GENERATED_BODY()

public:
    AMyMovingDirtyActor();

protected:
    virtual void Tick(float DeltaTime) override;
    virtual void BeginPlay() override;

public:

    UFUNCTION(BlueprintCallable)
    void SetTargetLocation(FVector NewTarget);

private:

    UPROPERTY(EditAnywhere)
    FVector TargetLocation;


    UPROPERTY(EditAnywhere)
    float MoveSpeed = 200.f;


    bool bIsDirty = true;


    FVector MoveDirection;

private:
    void RecalculateMovementIfDirty();
};

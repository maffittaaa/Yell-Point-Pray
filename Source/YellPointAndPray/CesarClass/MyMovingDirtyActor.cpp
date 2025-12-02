// Fill out your copyright notice in the Description page of Project Settings.


#include "CesarClass/MyMovingDirtyActor.h"

AMyMovingDirtyActor::AMyMovingDirtyActor()
{
    PrimaryActorTick.bCanEverTick = true;
}

void AMyMovingDirtyActor::BeginPlay()
{
    Super::BeginPlay();
    TargetLocation = GetActorLocation();
    SetTargetLocation(FVector(1000, 0, 0));
    SetTargetLocation(FVector(-1000, 0, 0));
    SetTargetLocation(FVector(-1000, 0, 0));
    SetTargetLocation(FVector(-1000, 0, 0));
    SetTargetLocation(FVector(-1000, 0, 0));
}

void AMyMovingDirtyActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    RecalculateMovementIfDirty();

    if (!MoveDirection.IsNearlyZero())
    {
        FVector NewLocation = GetActorLocation() + MoveDirection * MoveSpeed * DeltaTime;
        SetActorLocation(NewLocation);
    }
    if ((GetActorLocation() - TargetLocation).IsNearlyZero(10)) {
        SetTargetLocation(GetActorLocation());
    }
}

void AMyMovingDirtyActor::SetTargetLocation(FVector NewTarget)
{
    if (!TargetLocation.Equals(NewTarget, 0.1f))
    {
        TargetLocation = NewTarget;
        bIsDirty = true;
    }
}

void AMyMovingDirtyActor::RecalculateMovementIfDirty()
{
    if (!bIsDirty)
        return;

    FVector Current = GetActorLocation();
    FVector ToTarget = TargetLocation - Current;

    if (ToTarget.IsNearlyZero())
    {
        MoveDirection = FVector::ZeroVector;
    }
    else
    {
        MoveDirection = ToTarget.GetSafeNormal();
    }

    UE_LOG(LogTemp, Log, TEXT("Recalculated Movement Direction: %s"), *MoveDirection.ToString());
    bIsDirty = false;
}


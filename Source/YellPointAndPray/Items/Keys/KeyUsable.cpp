// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Keys/KeyUsable.h"
#include "Door.h"
#include "DrawDebugHelpers.h"
#include "YellPointAndPrayCharacter.h"
#include <Net/UnrealNetwork.h>

AKeyUsable::AKeyUsable()
{
    Name = "Key Usable";
    ID = 6;
}

void AKeyUsable::Use_Implementation(AActor* User)
{
    if (!HasAuthority()) return;
    UseReal(User, GetWorld());
}

void AKeyUsable::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AKeyUsable, KeyID);
}

void AKeyUsable::UseReal_Implementation(AActor* User, UWorld* World)
{
    UE_LOG(LogTemp, Warning, TEXT("LockPick Used CLIENTSIDE!"));

    // Get vector to do the ray
    FVector start;
    FRotator dir;

    AYellPointAndPrayCharacter* Player = Cast<AYellPointAndPrayCharacter>(User);
    if (!Player) return;

    Player->GetController()->GetPlayerViewPoint(start, dir);

    FVector end = start + (dir.Vector() * 300);

    // Not hit player
    FHitResult hit;
    FCollisionQueryParams params;
    params.AddIgnoredActor(Player);

    if (World->LineTraceSingleByChannel(hit, start, end, ECC_Visibility, params)) {
        if (AActor* hitObject = hit.GetActor()) {
            if (hitObject->IsA(ADoor::StaticClass()))
            {
                ADoor* Door = Cast<ADoor>(hitObject);
                if (Door)
                {
                    Door->KeyUnlockDoor(KeyID);
                }
            }
        }
    }
}

void AKeyUsable::BeginPlay()
{
    Super::BeginPlay();
    UE_LOG(LogTemp, Warning, TEXT("BeginPlay KeyID = %d"), KeyID);
}
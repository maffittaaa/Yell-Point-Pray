// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Keys/KeyUsable.h"
#include "Door.h"
#include "DrawDebugHelpers.h"
#include "YellPointAndPrayCharacter.h"

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

void AKeyUsable::UseReal_Implementation(AActor* User, UWorld* World)
{
    UE_LOG(LogTemp, Warning, TEXT("LockPick Used CLIENTSIDE!"));

    // Get vector to do the ray
    FVector start;
    FRotator dir;

    AYellPointAndPrayCharacter* Player = Cast<AYellPointAndPrayCharacter>(User);
    if (!Player) return;

    Player->GetController()->GetPlayerViewPoint(start, dir);

    UE_LOG(LogTemp, Warning, TEXT("LockPick Controller: %s | Role: %s | World: %s"),
        *Player->GetController()->GetName(),
        *UEnum::GetValueAsString(GetLocalRole()),
        *Player->GetWorld()->GetName());

    FVector end = start + (dir.Vector() * 300);

    // Not hit player
    FHitResult hit;
    FCollisionQueryParams params;
    params.AddIgnoredActor(Player);

    // ray
    FColor lineColor = FColor::Red;
    DrawDebugLine(World, start, end, lineColor, true, -1, 0, 1.0f);

    if (World->LineTraceSingleByChannel(hit, start, end, ECC_Visibility, params)) {
        if (AActor* hitObject = hit.GetActor()) {
            if (hitObject->IsA(ADoor::StaticClass()))
            {
                ADoor* Door = Cast<ADoor>(hitObject);
                if (Door)
                {
                    Door->UnlockDoor();
                }
            }
        }
    }
}
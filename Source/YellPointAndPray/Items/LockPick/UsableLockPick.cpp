// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/LockPick/UsableLockPick.h"
#include "Door.h"
#include "DrawDebugHelpers.h"
#include "YellPointAndPrayCharacter.h"
#include "Items/LockPick/LockPickMiniGame.h"
#include <Kismet/GameplayStatics.h>

AUsableLockPick::AUsableLockPick()
{
	Name = "LockPick Usable";
	ID = 3;
}

void AUsableLockPick::Use_Implementation(AActor* User)
{
    if (!HasAuthority()) return;
    UseReal(User, GetWorld(), Cast<AYellPointAndPrayPlayerController>(Cast<AYellPointAndPrayCharacter>(User)->GetController()));
}

void AUsableLockPick::UseReal_Implementation(AActor* User, UWorld* World, AYellPointAndPrayPlayerController* PlayerController)
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


    if (World->LineTraceSingleByChannel(hit, start, end, ECC_Visibility, params)) {
        if (AActor* hitObject = hit.GetActor()) {
            if (hitObject->IsA(ADoor::StaticClass()))
            {
                ADoor* Door = Cast<ADoor>(hitObject);
                if (Door && Door->GetLocked())
                {
                    //Door->UnlockDoor();
                    PlayerController->StartMinigame(MiniGameActorClass, World, User, Door);
                }
            }
        }
    }
}
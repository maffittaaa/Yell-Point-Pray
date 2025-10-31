// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/ToyHammer/ToyHammerUsable.h"
#include <YellPointAndPrayCharacter.h>

AToyHammerUsable::AToyHammerUsable()
{
	Name = "Toy Hammer Usable";
	ID = 1;
}

void AToyHammerUsable::Use_Implementation(AActor* User)
{
    AYellPointAndPrayCharacter* Player = Cast<AYellPointAndPrayCharacter>(User);
    if (!Player) return;

    AGuard* Guard = Player->CurrentGuard;
    if (Guard == nullptr) return;

    Player->Client_KnockGuard(Guard);
}
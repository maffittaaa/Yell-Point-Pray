// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/ToyHammer/ToyHammerUsable.h"

AToyHammerUsable::AToyHammerUsable()
{
	Name = "Toy Hammer Usable";
	ID = 1;
}

void AToyHammerUsable::Use_Implementation(AActor* User)
{
	UE_LOG(LogTemp, Warning, TEXT("ToyHammer Used CARALHOOO!"));
}
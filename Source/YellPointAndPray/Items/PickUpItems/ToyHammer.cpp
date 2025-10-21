// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/PickUpItems/ToyHammer.h"

AToyHammer::AToyHammer() 
{
	Name = "Toy Hammer that tp yha coiso cenas";
	ID = 1;
}


void AToyHammer::Use_Implementation(AActor* User)
{
	UE_LOG(LogTemp, Warning, TEXT("Item Used!"));
}

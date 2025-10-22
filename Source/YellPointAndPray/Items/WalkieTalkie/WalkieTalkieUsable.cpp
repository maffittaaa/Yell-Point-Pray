// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/WalkieTalkie/WalkieTalkieUsable.h"

AWalkieTalkieUsable::AWalkieTalkieUsable()
{
	Name = "WalkieTalkie Usable";
	ID = 2;
}

void AWalkieTalkieUsable::Use_Implementation(AActor* User)
{
	UE_LOG(LogTemp, Warning, TEXT("WalkieTalkie Used CARALHOOO!"));
}
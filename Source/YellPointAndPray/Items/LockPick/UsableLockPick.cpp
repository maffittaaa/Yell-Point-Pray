// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/LockPick/UsableLockPick.h"

AUsableLockPick::AUsableLockPick()
{
	Name = "LockPick Usable";
	ID = 3;
}

void AUsableLockPick::Use_Implementation(AActor* User)
{
	UE_LOG(LogTemp, Warning, TEXT("LockPick Used CARALHOOO!"));
}


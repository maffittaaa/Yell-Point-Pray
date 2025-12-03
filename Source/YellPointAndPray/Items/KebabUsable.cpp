// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/KebabUsable.h"
#include <YellPointAndPrayCharacter.h>

AKebabUsable::AKebabUsable() {
	Name = "Kebab Usable";
	ID = 7;
}

void AKebabUsable::Use_Implementation(AActor* User)
{
	if (!HasAuthority()) return;

	AYellPointAndPrayCharacter* Player = Cast<AYellPointAndPrayCharacter>(User);

	if (Player) 
	{
		Player->ChangeKebabEffect(true);
		UE_LOG(LogTemp, Warning, TEXT("Kebab Used CARALHOOO!"));
		Destroy();
	}
}
// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Kebab/KebabUsable.h"
#include <YellPointAndPrayCharacter.h>

AKebabUsable::AKebabUsable() {
	Name = "Kebab Usable";
	ID = 7;
}

void AKebabUsable::BeginPlay() {
	Super::BeginPlay();
	Name = "Kebab Usable";
	ID = 7;
}

void AKebabUsable::Use_Implementation(AActor* User)
{
	if (!HasAuthority()) return;

	AYellPointAndPrayCharacter* Player = Cast<AYellPointAndPrayCharacter>(User);

	if (Player) 
	{
		UE_LOG(LogTemp, Warning, TEXT("CurrentItemSelected: %d"), Player->InventoryComponent->CurrentItemSelected);
		Player->ChangeKebabEffect(true);
		UE_LOG(LogTemp, Warning, TEXT("Kebab Used CARALHOOO!"));
		Destroy();
	}
}
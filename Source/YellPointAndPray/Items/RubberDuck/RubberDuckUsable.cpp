// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/RubberDuck/RubberDuckUsable.h"



ARubberDuckUsable::ARubberDuckUsable() {

}

void ARubberDuckUsable::Use_Implementation(AActor* User) {
	UE_LOG(LogTemp, Warning, TEXT("Rubber Duck Used CARALHOOO!"));
}
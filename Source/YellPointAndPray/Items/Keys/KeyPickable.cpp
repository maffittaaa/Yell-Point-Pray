// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Keys/KeyPickable.h"
#include <Net/UnrealNetwork.h>

AKeyPickable::AKeyPickable() {
	Name = "Key Pickable";
	ID = 6;
}

void AKeyPickable::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AKeyPickable, KeyID);
}
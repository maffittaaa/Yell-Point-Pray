// Fill out your copyright notice in the Description page of Project Settings.


#include "Players/ACCheats.h"

#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UACCheats::UACCheats()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UACCheats::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UACCheats::TeleportToLaserRoom(AActor* character, FVector location) {
	character->SetActorLocation(location);
}

void UACCheats::TeleportToEletricalRoom(AActor* character, FVector location) {
	character->SetActorLocation(location);
}

void UACCheats::NotDetectedByGuard() {
	bool bNewState = !bNotDetectedByGuards;

	Server_SetNotDetectedByGuard(bNewState);

	bNotDetectedByGuards = bNewState;

	if (bNotDetectedByGuards) {
		UE_LOG(LogTemp, Warning, TEXT("Cheat ON - guards don't see players"));
	} else
		UE_LOG(LogTemp, Warning, TEXT("Cheat OFF - guards see players"));
}

void UACCheats::Server_SetNotDetectedByGuard_Implementation(bool bNewState) {
	bNotDetectedByGuards = bNewState;
    
	if (bNotDetectedByGuards) {
		UE_LOG(LogTemp, Warning, TEXT("Cheat ON (server) - guards don't see players"));
	} else
		UE_LOG(LogTemp, Warning, TEXT("Cheat OFF (server) - guards see players"));
}

void UACCheats::OnRep_NotDetectedByGuards() {
	UE_LOG(LogTemp, Warning, TEXT("Cheat state replicated: %hhd"), bNotDetectedByGuards);
}

void UACCheats::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UACCheats, bNotDetectedByGuards);
}


// Called every frame
void UACCheats::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}


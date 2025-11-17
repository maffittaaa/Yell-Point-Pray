#include "Players/ACCheats.h"
#include "Net/UnrealNetwork.h"

UACCheats::UACCheats() {
	PrimaryComponentTick.bCanEverTick = true;
}

void UACCheats::BeginPlay() {
	Super::BeginPlay();
}

void UACCheats::Server_TeleportToLaserRoom_Implementation(AActor* character, FVector location) {
	character->TeleportTo(location, FRotator(0,0,0));
}

void UACCheats::Server_TeleportToEletricalRoom_Implementation(AActor* character, FVector location) {
	character->TeleportTo(location, FRotator(0,0,0));
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

void UACCheats::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}


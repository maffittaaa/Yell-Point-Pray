// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/RubberDuck/RubberDuckUsable.h"
#include "YellPointAndPrayCharacter.h"
#include <Net/UnrealNetwork.h>

AYellPointAndPrayCharacter* player;

ARubberDuckUsable::ARubberDuckUsable() {
	PrimaryActorTick.bCanEverTick = true;
}


void ARubberDuckUsable::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARubberDuckUsable, AddForce);

}

void ARubberDuckUsable::Use_Implementation(AActor* User) {
	UE_LOG(LogTemp, Warning, TEXT("Rubber Duck Used CARALHOOO!"));
	player = Cast<AYellPointAndPrayCharacter>(User);
	AddForce = true;
	player->GetDuck(this);
}

void ARubberDuckUsable::Throw_Implementation(AActor* User, UWorld* World, TSubclassOf<AActor> NewHoldingItemClass, FVector NewPosition, FRotator dir) {

	if (!HasAuthority()) return;

	FActorSpawnParameters SpawnParams;
	AActor* SpawnedDuck = World->SpawnActor<AActor>(NewHoldingItemClass, NewPosition, FRotator::ZeroRotator, SpawnParams);
	if (!SpawnedDuck) return;

	SpawnedDuck->SetReplicates(true);
	SpawnedDuck->SetReplicateMovement(true);
	SpawnedDuck->bAlwaysRelevant = true;

	UPrimitiveComponent* PrimComp = Cast<UPrimitiveComponent>(SpawnedDuck->GetComponentByClass(UPrimitiveComponent::StaticClass()));
	if (PrimComp && PrimComp->IsSimulatingPhysics())
	{


		float ThrowStrength = 50000.0f;
		ThrowForce = 0;

		// Apply the impulse
		PrimComp->AddImpulse(dir.Vector() * ThrowStrength);
	}
}

void ARubberDuckUsable::ChangeAdd() {
	AddForce = false;
	player->ThrowDuck(this);
}

void ARubberDuckUsable::BeginPlay()
{
	Super::BeginPlay();
}

void ARubberDuckUsable::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	if (AddForce) {
		ThrowForce += DeltaTime * 10;
		if (ThrowForce >= 100) {
			ThrowForce = 100;
			AddForce = false;
			player->ThrowDuck(this);
		}
	}
}
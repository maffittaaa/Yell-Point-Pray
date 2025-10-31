// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/RubberDuck/RubberDuckUsable.h"
#include "YellPointAndPrayCharacter.h"
#include <Net/UnrealNetwork.h>

ARubberDuckUsable::ARubberDuckUsable() {
	PrimaryActorTick.bCanEverTick = true;
	PlayerActor = nullptr;
}

void ARubberDuckUsable::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARubberDuckUsable, AddForce);

}

void ARubberDuckUsable::Use_Implementation(AActor* User) {
	UE_LOG(LogTemp, Warning, TEXT("Rubber Duck Used CARALHOOO!"));
	PlayerActor = User;
	AYellPointAndPrayCharacter* Player = Cast<AYellPointAndPrayCharacter>(User);
	AddForce = true;
	if (Player)
	{
		Player->GetDuck(this);

	}
}

void ARubberDuckUsable::Throw_Implementation(AActor* User, UWorld* World, TSubclassOf<AActor> NewHoldingItemClass, FVector NewPosition, FRotator dir) {

	if (!HasAuthority()) return; 

	FActorSpawnParameters SpawnParams;
	AActor* SpawnedDuck = World->SpawnActor<AActor>(NewHoldingItemClass, NewPosition, FRotator::ZeroRotator, SpawnParams);
	if (!SpawnedDuck) 
	{
		return;
	} 

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

	if (PlayerActor)
	{
		AYellPointAndPrayCharacter* PlayerChar = Cast<AYellPointAndPrayCharacter>(PlayerActor);
		if (PlayerChar)
		{
			PlayerChar->ThrowDuck(this);
		}
	}
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
			if (PlayerActor)
			{
				AYellPointAndPrayCharacter* Player = Cast<AYellPointAndPrayCharacter>(PlayerActor);
				if (Player)
				{
					Player->ThrowDuck(this);
				}
			}	
		}
	}
}
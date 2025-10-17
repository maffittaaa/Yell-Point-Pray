// Fill out your copyright notice in the Description page of Project Settings.

#include "PickableItem.h"
#include "GameFramework/Character.h"
#include <Kismet/GameplayStatics.h>
#include "YellPointAndPrayCharacter.h"

// Sets default values
APickableItem::APickableItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;

	bReplicates = true;
}

// Called when the game starts or when spawned
void APickableItem::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void APickableItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APickableItem::Interact_Implementation(AActor* Interactor) {
	UE_LOG(LogTemp, Warning, TEXT("Item interacted with! :D"));
	Destroy();
}





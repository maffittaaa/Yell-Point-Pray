// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/UsableItemsParent/UsableItem.h"

// Sets default values
AUsableItem::AUsableItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshHoldingItem"));
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Mesh->SetupAttachment(RootComponent);
	
	bReplicates = true;
	bAlwaysRelevant = true;
}

// Called when the game starts or when spawned
void AUsableItem::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AUsableItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


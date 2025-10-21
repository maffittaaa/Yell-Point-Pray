// Fill out your copyright notice in the Description page of Project Settings.


#include "WhiteBoard.h"

// Sets default values
AWhiteBoard::AWhiteBoard()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
}

// Called when the game starts or when spawned
void AWhiteBoard::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AWhiteBoard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWhiteBoard::Interact_Implementation(AActor* Interactor) {
	UE_LOG(LogTemp, Warning, TEXT("Show Board"));
}

void AWhiteBoard::CloseBoard() {

}


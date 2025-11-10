// Fill out your copyright notice in the Description page of Project Settings.


#include "Obstacles/Laser/EletricBox.h"

// Sets default values
AEletricBox::AEletricBox()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AEletricBox::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEletricBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


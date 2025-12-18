// Fill out your copyright notice in the Description page of Project Settings.


#include "Menus/InGameMenu.h"

// Sets default values
AInGameMenu::AInGameMenu()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
}

// Called when the game starts or when spawned
void AInGameMenu::BeginPlay()
{
	Super::BeginPlay();

	GameOverWidget->AddToViewport();
	
}

void AInGameMenu::CloseMenu() {
	GameOverWidget->RemoveFromParent();


	Destroy();
}

// Called every frame
void AInGameMenu::Tick(float DeltaTime)
{

}

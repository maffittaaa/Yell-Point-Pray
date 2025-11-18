// Fill out your copyright notice in the Description page of Project Settings.


#include "ComicallyLargeButton.h"
#include <Net/UnrealNetwork.h>

// Sets default values
AComicallyLargeButton::AComicallyLargeButton()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	BigButtonMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BigRedButton"));
	BigButtonMesh->SetupAttachment(RootComponent);

	BaseButtonMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseButton"));
	BaseButtonMesh->SetupAttachment(RootComponent);

	SuportButtonMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SupportButton"));
	SuportButtonMesh->SetupAttachment(BaseButtonMesh);

	bReplicates = true;
}

// Called when the game starts or when spawned
void AComicallyLargeButton::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AComicallyLargeButton::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AComicallyLargeButton::OnRep_ButtonState()
{
	// This runs on all clients when ButtonUsed or ButtonAvailable changes

	UE_LOG(LogTemp, Warning, TEXT("On Rep Called interacted with! :D"));
	UE_LOG(LogTemp, Warning, TEXT("ButtonUsed: %d"), ButtonUsed);
	UE_LOG(LogTemp, Warning, TEXT("ButtonAvailable: %d"), ButtonAvailable);

	if (ButtonUsed)
	{
		BigButtonMesh->SetMaterial(0, AlreadyUsed);
	}
	else if (ButtonAvailable)
	{
		BigButtonMesh->SetMaterial(0, PreparedToBeUsed);
	}
	else
	{
		BigButtonMesh->SetMaterial(0, CantBeUsed);
	}
}

void AComicallyLargeButton::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AComicallyLargeButton, BigButtonMesh);
	DOREPLIFETIME(AComicallyLargeButton, ButtonUsed);
	DOREPLIFETIME(AComicallyLargeButton, ButtonAvailable);
}
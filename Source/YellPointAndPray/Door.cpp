// Fill out your copyright notice in the Description page of Project Settings.


#include "Door.h"
#include <Net/UnrealNetwork.h>

// Sets default values
ADoor::ADoor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	FrameMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FrameMeshComp"));
	FrameMeshComp->SetupAttachment(RootComponent);



	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(FrameMeshComp);

	DoorMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMeshComp"));
	DoorMeshComp->SetupAttachment(Mesh);


	bReplicates = true;
}

void ADoor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADoor, IsOpen);
	DOREPLIFETIME(ADoor, Play);
	DOREPLIFETIME(ADoor, Side);
}

// Called when the game starts or when spawned
void ADoor::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ADoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (Play == true) {
		FRotator Rotation = Mesh->GetRelativeRotation();
		Rotation.Yaw += 90 * DeltaTime * IsOpen;
		Mesh->SetRelativeRotation(Rotation);
		if (Rotation.Yaw > 90 && IsOpen == 1) {
			Rotation.Yaw = 90;
			Mesh->SetRelativeRotation(Rotation);
			Play = false;
			return;
		}
		if (Rotation.Yaw < -90 && IsOpen == 1) {
			Rotation.Yaw = -90;
			Mesh->SetRelativeRotation(Rotation);
			Play = false;
			return;
		}
		if (Rotation.Yaw < 0 && IsOpen == -1) {
			Rotation.Yaw = 0;
			Mesh->SetRelativeRotation(Rotation);
			Play = false;
			return;
		}
	}
	//FRotator Rotation = Mesh->GetRelativeRotation();
	//UE_LOG(LogTemp, Warning, TEXT("IsOpen: %d"), IsOpen);
	//Rotation.Yaw += 90 * DeltaTime * IsOpen;
	//Mesh->SetRelativeRotation(Rotation);

}

void ADoor::Interact_Implementation(AActor* Interactor) 
{
	//FVector DoorLoc = GetActorLocation();
	//FVector PlayerLoc = Interactor->GetActorLocation();

	//FVector Dir = PlayerLoc - DoorLoc;
	//FVector Forward = GetActorForwardVector();

	//UE_LOG(LogTemp, Warning, TEXT("DoorLoc: %s"), *DoorLoc.ToString());
	//UE_LOG(LogTemp, Warning, TEXT("PlayerLoc: %s"), *PlayerLoc.ToString());
	//UE_LOG(LogTemp, Warning, TEXT("Dir: %s"), *Dir.ToString());
	//UE_LOG(LogTemp, Warning, TEXT("Forward: %s"), *Forward.ToString());

	//float Dot = FVector::DotProduct(Dir, Forward);

	//if (Dot < 0.f) {
	//	Side = -1;
	//}
	//else {
	//	Side = 1;
	//}
	IsOpen = -IsOpen;
	//if (IsOpen == -1) {
	//	Side = 1;
	//}
	//UE_LOG(LogTemp, Warning, TEXT("Side: %d"), Side);
	//UE_LOG(LogTemp, Warning, TEXT("IsOpen: %d"), IsOpen);
	Play = true;
}




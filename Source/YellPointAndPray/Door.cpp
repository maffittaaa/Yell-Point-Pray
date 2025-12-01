// Fill out your copyright notice in the Description page of Project Settings.


#include "Door.h"
#include "Obstacles/Guard/Guard.h"
#include <Net/UnrealNetwork.h>
#include "Kismet/GameplayStatics.h"

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

	DoorNobMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorNobMesh"));
	DoorNobMesh->SetupAttachment(DoorMeshComp);

	bReplicates = true;

}

void ADoor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADoor, IsOpen);
	DOREPLIFETIME(ADoor, Play);
	DOREPLIFETIME(ADoor, Side);
	DOREPLIFETIME(ADoor, Locked);
	DOREPLIFETIME(ADoor, isInitiallyLocked);
}

// Called when the game starts or when spawned
void ADoor::BeginPlay()
{
	Super::BeginPlay();
	GetLockDoor();

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGuard::StaticClass(), FoundActors);
	for (auto& FoundActor : FoundActors)
	{
		DoorMeshComp->MoveIgnoreActors.Add(FoundActor);
		FrameMeshComp->MoveIgnoreActors.Add(FoundActor);
		Mesh->MoveIgnoreActors.Add(FoundActor);
		DoorNobMesh->MoveIgnoreActors.Add(FoundActor);
	}
}

void ADoor::MulticastDoor_Implementation(float FinalYaw)
{
	FRotator Rotation = Mesh->GetRelativeRotation();
	Rotation.Yaw = FinalYaw;
	Mesh->SetRelativeRotation(Rotation);
	Play = false;
}

// Called every frame
void ADoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!Play) return;

	FRotator Rotation = Mesh->GetRelativeRotation();
	float CurrentYaw = Rotation.Yaw;

	float TargetYaw;
	if (IsOpen == 1) {
		TargetYaw = 90.f * Side;
	}
	else {
		TargetYaw = 0;
	}

	float Speed = 90.f * DeltaTime;

	float Direction;
	if (TargetYaw > CurrentYaw) {
		Direction = 1;
	}
	else {
		Direction = -1;
	}

	Rotation.Yaw += Speed * Direction;

	if ((Direction > 0.f && Rotation.Yaw >= TargetYaw) || (Direction < 0.f && Rotation.Yaw <= TargetYaw))
	{
		Rotation.Yaw = TargetYaw;
		if (HasAuthority())
		{
			MulticastDoor(TargetYaw);
			Play = false;
		}
	}

	Mesh->SetRelativeRotation(Rotation);

}

void ADoor::Interact_Implementation(AActor* Interactor) 
{
	this->ServerInteract(Interactor);
}

void ADoor::ServerInteract_Implementation(AActor* Interactor) {
	if (Locked || !HasAuthority()) return;

	FVector DoorLoc = GetActorLocation();
	FVector PlayerLoc = Interactor->GetActorLocation();
	FVector Dir = PlayerLoc - DoorLoc;
	FVector Forward = GetActorForwardVector();

	float Dot = FVector::DotProduct(Dir, Forward);

	if (IsOpen == -1)
	{
		if (Dot < 0.f) {
			Side = 1;
		}
		else {
			Side = -1;
		}
	}

	IsOpen = -IsOpen;

	Play = true;
}

void ADoor::UnlockDoor_Implementation() {
	UE_LOG(LogTemp, Warning, TEXT("Door Unlocked."));
	Locked = false;
}

void ADoor::KeyUnlockDoor_Implementation(int KeyID) {
	if (KeyID == DoorID) {
		UE_LOG(LogTemp, Warning, TEXT("Door Unlocked."));
		Locked = false;
	}
}

void ADoor::Reset_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("Door reset called!"));
	LockDoor();
	CloseDoor();
}

void ADoor::CloseDoor_Implementation() {
	if (IsOpen == -1 || !HasAuthority()) return;

	if (IsOpen == 1)
	{
		Side = 1;
	}

	IsOpen = -IsOpen;

	Play = true;
}

void ADoor::LockDoor_Implementation() {
	
	if (isInitiallyLocked) 
	{
		UE_LOG(LogTemp, Warning, TEXT("Door Locked."));
		Locked = true;
	}
}

void ADoor::GetLockDoor_Implementation() {
	
	if (Locked)
	{
		UE_LOG(LogTemp, Warning, TEXT("Door Is Initially Locked."));
		isInitiallyLocked = true;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Door Is NOT Initially Locked."));
		isInitiallyLocked = false;
	}
}


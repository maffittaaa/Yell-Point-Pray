// Fill out your copyright notice in the Description page of Project Settings.

#include "Obstacles/Guard/Guard.h"
#include "Interfaces/Caughtable.h"
#include "YellPointAndPrayCharacter.h"
#include <Net/UnrealNetwork.h>

// Sets default values
AGuard::AGuard()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	SetReplicateMovement(true);

	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComp"));
	PawnSensingComp->SightRadius = 2000.f;
	PawnSensingComp->HearingThreshold = 1000.f;
	PawnSensingComp->SetPeripheralVisionAngle(45.f);
	
	UE_LOG(LogTemp, Warning, TEXT("GUARD"));
}

void AGuard::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGuard, Patroling);
	DOREPLIFETIME(AGuard, Seen);
	DOREPLIFETIME(AGuard, CurrentWaypoint);
	DOREPLIFETIME(AGuard, TargetPlayer);
	DOREPLIFETIME(AGuard, CurrentSuspicion);
	DOREPLIFETIME(AGuard, Knocked);
}

// Called when the game starts or when spawned
void AGuard::BeginPlay()
{
	Super::BeginPlay();

	AIController = Cast<AAIController>(GetController());
	if (PawnSensingComp)
	{
		PawnSensingComp->OnSeePawn.AddDynamic(this, &AGuard::SeenPlayer);
	}
	Patrol();
}

void AGuard::Reset_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("Guard-specific reset called!"));
}

// Called every frame
void AGuard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!HasAuthority()) return;

	if (Knocked) return;

	if (TargetPlayer) {
		float Distance = FVector::Dist(GetActorLocation(), TargetPlayer->GetActorLocation());

		if (!AIController->LineOfSightTo(TargetPlayer) || Distance > PawnSensingComp->SightRadius)
		{
			SuspiciousOf();
			Patroling = false;
			Seen = false;
			Suspicious = true;
			TargetPlayer = nullptr;
		}
	}
	if (Patroling == true) {
		AActor* CurrentPoint = Waypoints[CurrentWaypoint];
		if (CurrentPoint)
		{
			float Distance = FVector::Dist(GetActorLocation(), CurrentPoint->GetActorLocation());
			if (Distance < 100.f)
			{
				if (CurrentWaypoint == Waypoints.Num() - 1) {
					CurrentWaypoint = 0;
				}
				else {
					CurrentWaypoint++;
				}
				Patrol();
			}
		}
	}
	if (Suspicious == true) {
		//UE_LOG(LogTemp, Warning, TEXT("Suspicious Amount:  %d"), CurrentSuspicion)
		CurrentSuspicion -= DeltaTime;
		float Distance = FVector::Dist(GetActorLocation(), LastSeenLocation);
		if (Distance < 100.f) {
			LookAround(DeltaTime);
		}
		if (CurrentSuspicion <= 0) {
			ExtraRotation = 0;
			invert = 1;
			CurrentSuspicion = 0;
			Patrol();
			Patroling = true;
			Seen = false;
			Suspicious = false;
		}
	}

	if (Seen == true) {
		//UE_LOG(LogTemp, Warning, TEXT("Suspicious Amount:  %d"), CurrentSuspicion)
		FVector Direction = TargetPlayer->GetActorLocation() - GetActorLocation();
		FRotator NewRotation = FRotationMatrix::MakeFromX(Direction).Rotator();
		SetActorRotation(NewRotation);
		CurrentSuspicion += 90 * DeltaTime;
		if (CurrentSuspicion >= 100) {
			if (TargetPlayer && TargetPlayer->GetClass()->ImplementsInterface(UCaughtable::StaticClass()))
			{
				ICaughtable::Execute_Caught(TargetPlayer);
				CurrentSuspicion = 0;
			}
		}
	}
}

void AGuard::SeenPlayer(APawn* Pawn) {
	if (!HasAuthority()) return;
 	//UE_LOG(LogTemp, Warning, TEXT("Saw Player!"));
	TargetPlayer = Pawn;
	LastSeenLocation = TargetPlayer->GetActorLocation();
	Patroling = false;
	Seen = true;
	Suspicious = false;
	ExtraRotation = 0;
	invert = 1;
	AIController->StopMovement();
}

void AGuard::Patrol() {
	if (!HasAuthority()) return;

	AActor* NextPoint = Waypoints[CurrentWaypoint];
	if (AIController && NextPoint)
	{
		AIController->MoveToActor(NextPoint);
	}
}

void AGuard::SuspiciousOf() {
	AIController->MoveToLocation(LastSeenLocation);
}

void AGuard::Knock_Implementation()
{
	KnockMySelf();
}

void AGuard::ServerKnock_Implementation()
{
	if (!HasAuthority()) return;

	Knocked = true;

	if (AIController)
	{
		AIController->StopMovement();
		//Destroy();
		UE_LOG(LogTemp, Warning, TEXT("Guard was Knocked"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Guard was Knocked, but no AIController found"));
	}
}

bool AGuard::ServerKnock_Validate()
{
	return true;
}

void AGuard::KnockMySelf()
{
	// Only clients should call server RPCs
	if (!HasAuthority() && GetNetMode() != NM_Standalone)
	{
		ServerKnock();
	}
	else
	{
		// If we're on server or in standalone, call directly
		ServerKnock_Implementation();
	}
}

void AGuard::LookAround(float DeltaTime) 
{
	FRotator CurrentRotation = GetActorRotation();

	if (ExtraRotation >= 90) {
		invert = -1;
		ExtraRotation = 90;
	}
	if (ExtraRotation <= -90) {
		ExtraRotation = 0;
		invert = 1;
		CurrentSuspicion = 0;
		return;
	}

	float RotationThisFrame = 100.f * DeltaTime * invert; // incremental rotation
	ExtraRotation += RotationThisFrame;

	CurrentRotation.Yaw += RotationThisFrame;
	SetActorRotation(CurrentRotation);
}
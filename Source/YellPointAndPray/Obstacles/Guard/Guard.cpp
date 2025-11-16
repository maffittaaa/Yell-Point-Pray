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

	suspiciousMark = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Suspicious Mark"));
	suspiciousMark->SetupAttachment(RootComponent);
	suspiciousMark->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	suspiciousMark->SetHiddenInGame(true);
	
	alertedMark = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Alerted Mark"));
	alertedMark->SetupAttachment(RootComponent);
	alertedMark->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	alertedMark->SetHiddenInGame(true);
}

void AGuard::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGuard, Patroling);
	DOREPLIFETIME(AGuard, Seen);
	DOREPLIFETIME(AGuard, CurrentWaypoint);
	DOREPLIFETIME(AGuard, TargetPlayer);
	DOREPLIFETIME(AGuard, Suspicious);
	DOREPLIFETIME(AGuard, CurrentSuspicion);
	DOREPLIFETIME(AGuard, Knocked);
	DOREPLIFETIME(AGuard, World);
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

	StartLocation = GetActorLocation();
	StartRotation = GetActorTransform().GetRotation();
}

void AGuard::Reset_Implementation()
{
	UnKnockMySelf();
	FRotator rotation = StartRotation.Rotator();
	TeleportTo(StartLocation, rotation);
	UE_LOG(LogTemp, Warning, TEXT("Guard-specific reset called!"));
}

// Called every frame
void AGuard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	World = GetWorld();
	if (!HasAuthority()) return;

	if (Knocked) return;

	OpenDoors();
	CloseDoors(DeltaTime);

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
		//UE_LOG(LogTemp, Warning, TEXT("Suspicious Amount:  %f"), CurrentSuspicion)
		CurrentSuspicion -= DeltaTime * 10;
		suspiciousMark->SetHiddenInGame(false);
		float Distance = FVector::Dist(GetActorTransform().GetLocation(), LastSeenLocation);
		UE_LOG(LogTemp, Warning, TEXT("Distance Amount:  %f"), Distance)
		if (Distance < 200.f) {
			UE_LOG(LogTemp, Warning, TEXT("in  "));
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
			suspiciousMark->SetHiddenInGame(true);
		}
	}
	//FVector Dir = this->GetVelocity().GetSafeNormal();
	//FRotator NewRotation = FRotationMatrix::MakeFromX(Dir).Rotator();
	//SetActorRotation(NewRotation);
	if (Seen == true) {
		//UE_LOG(LogTemp, Warning, TEXT("Suspicious Amount:  %f"), CurrentSuspicion);
		if (TargetPlayer) {
			AYellPointAndPrayCharacter* playerCharacter = Cast<AYellPointAndPrayCharacter>(TargetPlayer);
			if (playerCharacter && playerCharacter->cheatsComponent && playerCharacter->cheatsComponent->bNotDetectedByGuards)
				return;
		}
		FVector Direction = TargetPlayer->GetActorLocation() - GetActorLocation();
		FRotator NewRotation = FRotationMatrix::MakeFromX(Direction).Rotator();
		SetActorRotation(NewRotation);
		CurrentSuspicion += 30 * DeltaTime;
		suspiciousMark->SetHiddenInGame(true);
		alertedMark->SetHiddenInGame(false);
		if (CurrentSuspicion >= 100) {
			if (TargetPlayer && TargetPlayer->GetClass()->ImplementsInterface(UCaughtable::StaticClass()))
			{
				CurrentSuspicion = 0;
				ICaughtable::Execute_Caught(TargetPlayer);
			}
		}
	}
}

void AGuard::Called(FVector Location)
{
	if (HasAuthority())
	{
		LastSeenLocation = Location;
		CurrentSuspicion = 70;
		Suspicious = true;
		Patroling = false;
		Seen = false;

		SuspiciousOf();
	}
}


void AGuard::OpenDoors()
{
	if (!World) return;

	FVector dir = GetActorForwardVector();

	FVector start = GetActorLocation();

	FHitResult hit;
	FCollisionQueryParams params;
	params.AddIgnoredActor(this);

	//ray
	if (World->LineTraceSingleByChannel(hit, start, start + dir * 300, ECC_Visibility, params)) {
		DrawDebugLine(World, start, start + dir * 100, FColor::Red, false, -1.0f, 0, 1.0f);
		if (AActor* hitObject = hit.GetActor())
		{
			if (hitObject->IsA(ADoor::StaticClass())) {
				if (hitObject->GetClass()->ImplementsInterface(UInteractable::StaticClass())) {
					if (LastOpen == hitObject) return;
					if (LastOpen != nullptr) {
						CloseTimer = 3;
						CloseDoors(0.f);
					}
					LastOpen = hitObject;
					IInteractable::Execute_Interact(hitObject, this);
				}
			}
		}
	}
}


void AGuard::CloseDoors(float DeltaTime)
{
	if (LastOpen != nullptr) {
		CloseTimer += DeltaTime;
		if (CloseTimer >= 3) {
			IInteractable::Execute_Interact(LastOpen, this);
			LastOpen = nullptr;
			CloseTimer = 0;
		}
	}
}

void AGuard::SeenPlayer(APawn* Pawn) {
	if (!HasAuthority()) return;

	AYellPointAndPrayCharacter* playerCharacter = Cast<AYellPointAndPrayCharacter>(Pawn);
	
	if (!playerCharacter)
		return;
	if (!playerCharacter->cheatsComponent)
		return;
	
	if (playerCharacter->cheatsComponent->bNotDetectedByGuards) {
		UE_LOG(LogTemp, Warning, TEXT("Guard saw player but cheat is active - ignoring detection"));
		return;
	}
	
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

void AGuard::ServerUnKnock_Implementation()
{
	if (!HasAuthority()) return;

	Knocked = false;
	Patroling = true;
	Seen = false;
	Suspicious = false;
	TargetPlayer = nullptr;
	CurrentSuspicion = 0;
}

bool AGuard::ServerUnKnock_Validate()
{
	return true;
}

void AGuard::UnKnockMySelf()
{
	// Only clients should call server RPCs
	if (!HasAuthority() && GetNetMode() != NM_Standalone)
	{
		ServerUnKnock();
	}
	else
	{
		// If we're on server or in standalone, call directly
		ServerUnKnock_Implementation();
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

	float RotationThisFrame = 100.f * DeltaTime * invert;
	UE_LOG(LogTemp, Warning, TEXT("AAAAAAAAAAAAAAAAAAAt:  %f"), RotationThisFrame)
	ExtraRotation += RotationThisFrame;

	CurrentRotation.Yaw += RotationThisFrame;
	SetActorRotation(CurrentRotation);
}
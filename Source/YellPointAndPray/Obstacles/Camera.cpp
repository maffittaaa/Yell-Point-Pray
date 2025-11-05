// Fill out your copyright notice in the Description page of Project Settings.

#include "Camera.h"
#include "Interfaces/Caughtable.h"
#include "Kismet/KismetMathLibrary.h"

ACamera::ACamera() {
	PrimaryActorTick.bCanEverTick = true;
	
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));

	skeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Skeletal Mesh Component"));
	skeletalMeshComponent->SetupAttachment(RootComponent);

	collisionCone = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Collision Cone"));
	collisionCone->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	collisionCone->OnComponentBeginOverlap.AddDynamic(this, &ACamera::OnOverlapBegin);
	collisionCone->OnComponentEndOverlap.AddDynamic(this, &ACamera::OnOverlapEnd);

	suspiciousMark = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Suspicious Mark"));
	suspiciousMark->SetupAttachment(RootComponent);
	suspiciousMark->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	suspiciousMark->SetHiddenInGame(true);
	
	alertedMark = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Alerted Mark"));
	alertedMark->SetupAttachment(RootComponent);
	alertedMark->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	alertedMark->SetHiddenInGame(true);

	SuspicionMax = 100;
}

void ACamera::BeginPlay() {
	Super::BeginPlay();

	if (collisionCone && skeletalMeshComponent)
			collisionCone->AttachToComponent(skeletalMeshComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("CameraSight"));
}

void ACamera::Reset_Implementation() {
	UE_LOG(LogTemp, Warning, TEXT("Camera-specific reset called!"));
	ResumeCameraAnimation();
	ResetCameraObstacle();
}

void ACamera::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {

	if (OtherActor->GetClass()->ImplementsInterface(UCaughtable::StaticClass())) {
		UE_LOG(LogTemp, Warning, TEXT("CAMERA SEEN"));

		if (!bIsAnimationStopped) {
			StopAnimation();
			collisionCone->SetMaterial(0, detectedMaterialInstance);
			FVector DirectionToPlayer = (OtherActor->GetActorLocation() - GetActorLocation()).GetSafeNormal();
			StoppedRotation = DirectionToPlayer.Rotation();
			DetectedPlayer = OtherActor;
		}
			
		if (PlayersNotSeenList.Contains(OtherActor)) {
			PlayersSeenList.Add(OtherActor, *(PlayersNotSeenList.Find(OtherActor)));
			PlayersNotSeenList.Remove(OtherActor);
			AmountOfPlayers++;
			if (AmountOfPlayers > 3)
				AmountOfPlayers = 3;
		} else {
			PlayersSeenList.Add(OtherActor, 0);
			AmountOfPlayers++;
			if (AmountOfPlayers > 3)
				AmountOfPlayers = 3;
		}
	}
}

void ACamera::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {
	if (OtherActor->GetClass()->ImplementsInterface(UCaughtable::StaticClass())) {
		UE_LOG(LogTemp, Warning, TEXT("I LEFT"));
		PlayersNotSeenList.Add(OtherActor, *(PlayersSeenList.Find(OtherActor)));
		PlayersSeenList.Remove(OtherActor);
		AmountOfPlayers--;
		if (AmountOfPlayers < 0) AmountOfPlayers = 0;
	}
}

void ACamera::PlayerInVision(float DeltaTime) {
	if (AmountOfPlayers > 0) {
		for (auto& Elem : PlayersSeenList) {
			suspiciousMark->SetHiddenInGame(false);
			collisionCone->SetMaterial(0, detectedMaterialInstance);
			suspiciousAmount = Elem.Value + (70 * DeltaTime * AmountOfPlayers);
			Elem.Value = suspiciousAmount;
			UE_LOG(LogTemp, Warning, TEXT("Suspicious Amount:  %d"), (int)Elem.Value);
			if (Elem.Value >= SuspicionMax) {
				suspiciousMark->SetHiddenInGame(true);
				alertedMark->SetHiddenInGame(false);
				ICaughtable::Execute_Caught(Elem.Key);
				AmountOfPlayers = 0;
			}
		}
	}
}

void ACamera::NoPlayerInVision(float DeltaTime) {
	TArray<AActor*> ToDelete;
	
	for (auto& Elem : PlayersNotSeenList) {
		suspiciousAmount = Elem.Value - (50 * DeltaTime);
		Elem.Value = suspiciousAmount;
		UE_LOG(LogTemp, Warning, TEXT("Suspicious Amount:  %d"), (int)Elem.Value);
		if (Elem.Value <= 0) {
			suspiciousMark->SetHiddenInGame(true);
			alertedMark->SetHiddenInGame(true);
			ToDelete.Add(Elem.Key);
			continue;
		}
	}
	
	for (int i = 0; i < ToDelete.Num(); i++) {
		PlayersNotSeenList.Remove(ToDelete[i]);
	}

	if (PlayersSeenList.Num() == 0 && PlayersNotSeenList.Num() == 0 && bIsAnimationStopped)
		ResumeCameraAnimation();
}

void ACamera::StopAnimation() {
	if (skeletalMeshComponent && skeletalMeshComponent->GetAnimInstance()) {
		skeletalMeshComponent->SetAnimationMode(EAnimationMode::AnimationSingleNode);
		skeletalMeshComponent->Stop();
		bIsAnimationStopped = true;
		UE_LOG(LogTemp, Warning, TEXT("Camera animation stopped"));
	}
}


void ACamera::ResumeCameraAnimation() {
	if (skeletalMeshComponent) {
		skeletalMeshComponent->SetAnimationMode(EAnimationMode::AnimationBlueprint);
		skeletalMeshComponent->SetAnimInstanceClass(skeletalMeshComponent->GetAnimClass());
	}

	bIsAnimationStopped = false;
	DetectedPlayer = nullptr;
	collisionCone->SetMaterial(0, undetectedMaterialInstance);
}

void ACamera::ResetCameraObstacle() {
	suspiciousMark->SetHiddenInGame(true);
	alertedMark->SetHiddenInGame(true);
	suspiciousAmount = 0.0f;
}

void ACamera::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	
	if (bIsAnimationStopped)
		SetActorRotation(StoppedRotation);
	
	PlayerInVision(DeltaTime);
	NoPlayerInVision(DeltaTime);
}


// Fill out your copyright notice in the Description page of Project Settings.

#include "Camera.h"
#include "Interfaces/Caughtable.h"
#include "Kismet/KismetMathLibrary.h"
#include <Net/UnrealNetwork.h>

ACamera::ACamera() {
	PrimaryActorTick.bCanEverTick = true;
	
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));

	skeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Skeletal Mesh Component"));
	skeletalMeshComponent->SetupAttachment(RootComponent);

	collisionCone = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Collision Cone"));
	collisionCone->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	collisionCone->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	collisionCone->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	
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

	CameraAudioPlayer = CreateDefaultSubobject<UAudioComponent>(TEXT("BonkAudioPlayer"));
	CameraAudioPlayer->SetupAttachment(skeletalMeshComponent);
}

void ACamera::BeginPlay() {
	Super::BeginPlay();

	ChangeCanPlaySound(true);

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
		
		StopAnimation();
		collisionCone->SetMaterial(0, detectedMaterialInstance);
	}
}

void ACamera::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {
	if (OtherActor->GetClass()->ImplementsInterface(UCaughtable::StaticClass())) {
		if (PlayersSeenList.Contains(OtherActor)) {
			UE_LOG(LogTemp, Warning, TEXT("I LEFT"));
			PlayersNotSeenList.Add(OtherActor, *(PlayersSeenList.Find(OtherActor)));
			PlayersSeenList.Remove(OtherActor);
			AmountOfPlayers--;
			if (AmountOfPlayers < 0)
				AmountOfPlayers = 0;
		}
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

	//SOUND PLAY
	ChangeCanPlaySound(true);

	if (!CameraAudioPlayer->IsPlaying()) 
	{
		//CameraAudioPlayer->Play();
	}
	
	for (int i = 0; i < ToDelete.Num(); i++) {
		PlayersNotSeenList.Remove(ToDelete[i]);
	}

	if (PlayersSeenList.Num() == 0 && PlayersNotSeenList.Num() == 0 && bIsAnimationStopped)
		ResumeCameraAnimation();
}

void ACamera::StopAnimation() {
	if (skeletalMeshComponent && skeletalMeshComponent->GetAnimInstance()) 
	{
		bIsAnimationStopped = true;
		skeletalMeshComponent->SetComponentTickEnabled(false);
	
		//SOUND STOP
		ChangeCanPlaySound(false);

		if (CameraAudioPlayer->IsPlaying())
		{
			//CameraAudioPlayer->Stop();
		}
	}
}


void ACamera::ResumeCameraAnimation() {
	bIsAnimationStopped = false;
	collisionCone->SetMaterial(0, undetectedMaterialInstance);
	skeletalMeshComponent->SetComponentTickEnabled(true);
	
	//SOUND PLAY
	ChangeCanPlaySound(true);
	
	if (!CameraAudioPlayer->IsPlaying())
	{
		//CameraAudioPlayer->Play();
	}
}

void ACamera::ChangeCanPlaySound_Implementation(bool state) 
{
	CanPlaySound = true;
}

void ACamera::ResetCameraObstacle() {
	suspiciousMark->SetHiddenInGame(true);
	alertedMark->SetHiddenInGame(true);
	suspiciousAmount = 0.0f;
}

void ACamera::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	
	PlayerInVision(DeltaTime);
	
	if (PlayersNotSeenList.Num() > 0)
		NoPlayerInVision(DeltaTime);
}

void ACamera::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACamera, CanPlaySound);
	DOREPLIFETIME(ACamera, CameraAudioPlayer);
}
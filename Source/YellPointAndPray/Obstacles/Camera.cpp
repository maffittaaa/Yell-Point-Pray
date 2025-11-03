// Fill out your copyright notice in the Description page of Project Settings.

#include "Camera.h"
#include "Interfaces/Caughtable.h"
#include "Kismet/KismetMathLibrary.h"

ACamera::ACamera() {
	PrimaryActorTick.bCanEverTick = true;
	
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	staticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	staticMeshComponent->SetupAttachment(RootComponent);

	// dynamicMesh = dynamicMeshComponent->GetDynamicMesh();

	SuspicionMax = 100;
}

void ACamera::BeginPlay() {
	Super::BeginPlay();

	LoadVisionMesh();
}

void ACamera::LoadVisionMesh() {
	TArray<FHitResult> hitResults = DoLineTraces();
	// TArray<FVector2D> vertices = HitResultsTo2DVertices(hitResults);
}

TArray<FHitResult> ACamera::DoLineTraces() {
	
	TArray<FHitResult> HitResults;
	
	int numberOfTimes = FMath::CeilToInt(visionArc / testVisionArc);

	for (int i = 0; i < numberOfTimes; i++) {
		FVector start = GetActorLocation();
		FVector forwardVector = GetActorForwardVector();
		
		float angleDeg = UKismetMathLibrary::Clamp(
			(i * testVisionArc * 2.0f) - visionArc,
			-visionArc,
			visionArc
			);
		
		FVector rotatedVector = forwardVector.RotateAngleAxis(angleDeg, FVector(0.0f, 0.0f, 1.0f));
		FVector end = start + (rotatedVector * visionLength);

		UKismetMathLibrary::Dot_VectorVector(start, end);

		ECollisionChannel traceChannel = ECC_Visibility;
		FCollisionQueryParams RV_TraceParams = FCollisionQueryParams(FName(TEXT("RV_Trace")), false, this);
		RV_TraceParams.bTraceComplex = true;
		RV_TraceParams.bReturnPhysicalMaterial = false;
		RV_TraceParams.AddIgnoredActor(this);

		TArray<FHitResult> MultiHit;
		
		DrawDebugLine(GetWorld(), start, end, FColor::Red, false, -1.0f, 0, 1.0f);

		bool bHit = GetWorld()->LineTraceMultiByChannel(
			MultiHit,
			start,
			end,
			traceChannel,
			RV_TraceParams
		);

		if (bHit)
			HitResults.Append(MultiHit);
	}
	return HitResults;
}

// TArray<FVector2D> ACamera::HitResultsTo2DVertices(TArray<FHitResult>& hitResults) {
// 	TArray<FVector2D> hitVector;
// 	
// 	for (FHitResult hitResult : hitResults) {
// 		FVector location = hitResult.Location;
// 		FVector traceEnd = hitResult.TraceEnd;
// 		
// 		FVector finalPosition = hitResult.bBlockingHit ? location : traceEnd;
// 		FVector localLocation = UKismetMathLibrary::InverseTransformLocation(GetActorTransform(), finalPosition);
// 		FVector2D localLocation2D = FVector2D(localLocation.X, localLocation.Y);
// 		
// 		hitVector.Add(localLocation2D);
// 	}
// 	
// 	return hitVector;
// }

void ACamera::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {

	if (OtherActor->GetClass()->ImplementsInterface(UCaughtable::StaticClass())) {
		UE_LOG(LogTemp, Warning, TEXT("CAMERA SEEN"));
		if (PlayersNotSeenList.Contains(OtherActor)) {
			PlayersSeenList.Add(OtherActor, *(PlayersNotSeenList.Find(OtherActor)));
			PlayersNotSeenList.Remove(OtherActor);
			AmountOfPlayers++;
			if (AmountOfPlayers > 3) AmountOfPlayers = 3;
		} else {
			PlayersSeenList.Add(OtherActor, 0);
			AmountOfPlayers++;
			if (AmountOfPlayers > 3) AmountOfPlayers = 3;
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
			float SuspiciousAmount = Elem.Value + (70 * DeltaTime * AmountOfPlayers);
			Elem.Value = SuspiciousAmount;
			UE_LOG(LogTemp, Warning, TEXT("Suspicious Amount:  %d"), (int)Elem.Value);
			if (Elem.Value >= SuspicionMax) {
				ICaughtable::Execute_Caught(Elem.Key);
				AmountOfPlayers = 0;
			}
		}
	}
}

void ACamera::NoPlayerInVision(float DeltaTime) {
	TArray<AActor*> ToDelete;
	
	for (auto& Elem : PlayersNotSeenList) {
		float SuspiciousAmount = Elem.Value - (50 * DeltaTime);
		Elem.Value = SuspiciousAmount;
		UE_LOG(LogTemp, Warning, TEXT("Suspicious Amount:  %d"), (int)Elem.Value);
		if (Elem.Value <= 0) {
			ToDelete.Add(Elem.Key);
			continue;
		}
	}
	
	for (int i = 0; i < ToDelete.Num(); i++) {
		PlayersNotSeenList.Remove(ToDelete[i]);
	}
}

// Called every frame
void ACamera::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	PlayerInVision(DeltaTime);
	NoPlayerInVision(DeltaTime);

	LoadVisionMesh();
}


// Fill out your copyright notice in the Description page of Project Settings.

#include "Camera.h"
#include "Interfaces/Caughtable.h"
#include "Kismet/KismetMathLibrary.h"

ACamera::ACamera() {
	PrimaryActorTick.bCanEverTick = true;
	
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	dynamicMeshComponent = CreateDefaultSubobject<UDynamicMeshComponent>(TEXT("MeshComp"));
	dynamicMeshComponent->SetupAttachment(RootComponent);

	dynamicMesh = dynamicMeshComponent->GetDynamicMesh();

	TArray<FVector2D> vertices;
	vertices.Add(FVector2D(0.0f, 0.0f));
	vertices.Add(FVector2D(100.0f, 50.0f));
	vertices.Add(FVector2D(100.0f, -50.0f));;

	FTransform transform = FTransform::Identity;
	bool bAllowSelfIntersections = true;
	UGeometryScriptDebug* debugScript = nullptr;

	UGeometryScriptLibrary_MeshPrimitiveFunctions::AppendTriangulatedPolygon(
		dynamicMesh,
		FGeometryScriptPrimitiveOptions(),
		transform,
		vertices,
		bAllowSelfIntersections,
		debugScript
		);

	SuspicionMax = 100;
}

void ACamera::BeginPlay() {
	Super::BeginPlay();
	
}

FHitResult ACamera::DoLineTraces() {
	
	FHitResult RV_Hit;
	
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

		ECollisionChannel traceChannel = ECC_Visibility;
		FCollisionQueryParams RV_TraceParams = FCollisionQueryParams(FName(TEXT("RV_Trace")), false, this);
		RV_TraceParams.bTraceComplex = false;
		RV_TraceParams.bReturnPhysicalMaterial = false;
		RV_TraceParams.AddIgnoredActor(this);
		
		DrawDebugLine(GetWorld(), start, end, FColor::Red, false, -1.0f, 0, 1.0f);

		bool bHit = GetWorld()->LineTraceSingleByChannel(
			RV_Hit,
			start,
			end,
			traceChannel,
			RV_TraceParams
		);
	}
	return 
}


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
	DoLineTraces();
}

